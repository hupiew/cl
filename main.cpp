// main.cpp
/* CL
   Copyright (C) 2022  Hupie (hupiew[at]gmail.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <iostream>
#include <memory>

#include <QByteArray>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QMimeDatabase>

#include <cedd.h>
#include <cl.h>
#include <extractor.h>
#include <fcth.h>
#include <jcd.h>
#include <phash.h>
#include <util/IscFileHelper.hpp>
#include <videoextractor.h>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("cl");
    QCoreApplication::setApplicationVersion("1.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Extract image descriptors.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(
        "images/videos",
        QCoreApplication::translate("main", "Images or video to process."));
    QCommandLineOption hasherOption(
        QStringList() << "d"
                      << "hasher",
        QCoreApplication::translate(
            "main", "Which image descriptor to use. cl, cedd, fcth, jcd or phash"),
        "hasher",
        "cl");
    QCommandLineOption nameOption(
        QStringList() << "n"
                      << "name",
        QCoreApplication::translate(
            "main", "Name field of the Isc File, default is empty string."),
        "name",
        "");
    QCommandLineOption titleOption(
        QStringList() << "no-title",
        QCoreApplication::translate(
            "main", "Don't use input files as titles in the Isc Index."));
    QCommandLineOption makeIscOption(
        QStringList() << "isc"
                      << "Produce an Isc file to be loaded by ImSearch.");
    parser.addOption(hasherOption);
    parser.addOption(nameOption);
    parser.addOption(titleOption);
    parser.addOption(makeIscOption);

    parser.process(app);
    const QString hasher = parser.value(hasherOption).toLower();
    const QStringList args = parser.positionalArguments();

    // Set-up isc
    imsearch::IscFileHelper isc{};
    isc.set_name(parser.value(nameOption).toStdString());
    const bool make_isc = parser.isSet(makeIscOption);
    const bool no_title = parser.isSet(titleOption);

    std::unique_ptr<Extractor> extractor;
    uint8_t type = 255;
    if (hasher == "cl")
    {
        extractor = std::make_unique<ColorLayoutExtractor>();
        type = 2;
    }
    else if (hasher == "cedd")
    {
        extractor = std::make_unique<CEDD>();
        type = 4;
    }
    else if (hasher == "fcth")
    {
        extractor = std::make_unique<FCTH>();
        type = 5;
    }
    else if (hasher == "jcd")
    {
        extractor = std::make_unique<JCD>();
        type = 6;
    }
    else if (hasher == "phash")
    {
        extractor = std::make_unique<PHash>();
        type = 1;
    }
    else
    {
        std::cerr << "Unsupported hasher." << '\n';
        std::terminate();
    }
    for (const auto& item: args)
    {
        QMimeDatabase db;
        auto mime = db.mimeTypeForFile(item).name();

        if (mime.startsWith("video"))
        {
            if (!make_isc)
            {
                std::cerr << "Video hashing only works with the --isc option" << '\n';
                std::terminate();
            }
            auto cl = VideoExtractor();
            cl.set_extractor(std::move(extractor));
            const auto url = QUrl::fromLocalFile(item);
            {
                QEventLoop loop;
                QObject::connect(
                    &cl, &VideoExtractor::processingDone, &loop, &QEventLoop::quit);
                cl.load_video(url);
                loop.exec();
            }
            extractor = cl.get_extractor();
            isc.add_index(cl.get_data(!no_title, type));
            continue;
        }
        else if (mime.startsWith("image"))
        {
            const char format = QImage::Format_RGB32;
            const auto im = QImage(item, &format);
            if (im.isNull())
            {
                std::cout << "Can't load image: " << item.toStdString() << '\n';
                continue;
            }
            extractor->extract(im);
        }
        auto des = extractor->get_descriptor();

        if (make_isc)
        {
            imsearch::IscIndexHelper isc_index{};
            isc_index.add_hashdata(imsearch::BytesView(des));
            // isc_index.add_imageindex({0, 0}); // Not needed I think.
            // isc_index.add_timeindex({}); // Not needed I think.
            isc_index.set_length(1);
            isc_index.set_type(type);
            if (!no_title) isc_index.add_title(item.toStdString(), 0);
            isc.add_index(isc_index);
        }
        else
        {
            auto ar = QByteArray(reinterpret_cast<char*>(des.data()), des.size());

            auto j = ar.toBase64(QByteArray::Base64Encoding);

            auto s = std::string(j.data(), j.size());

            std::cout << item.toStdString() << ": " << s << '\n';
        }
    }
    if (make_isc)
    {
        const auto data = isc.to_file();
        const auto bytes =
            QByteArray(reinterpret_cast<const char*>(data.data()), data.size());
        auto file = QFile{ "output.isc" };
        if (file.open(QIODevice::ReadWrite))
        {
            QDataStream stream(&file);
            stream << bytes;
        }
    }
    //return a.exec();
}
