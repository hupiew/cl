// main.cpp
/* CL
   This file was originally written in Java as part of LIRE,
   and was adapted into C++ by Hupie.

   Copyright (C) 2002-2013 Mathias Lux (mathias@juggle.at)
     http://www.semanticmetadata.net/lire, http://www.lire-project.net
       Mathias Lux, Oge Marques. Visual Information Retrieval using Java and LIRE
       Morgan & Claypool, 2013
       URL: http://www.morganclaypool.com/doi/abs/10.2200/S00468ED1V01Y201301ICR025

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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QByteArray>
#include <QImage>
#include <QImageReader>

#include <cedd.h>
#include <cl.h>
#include <fcth.h>
#include <jcd.h>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("cl");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Extract image descriptors.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("images", QCoreApplication::translate("main", "Images to process."));
    QCommandLineOption hasherOption(
        QStringList() << "d"
                      << "hasher",
        QCoreApplication::translate(
            "main", "Which image descriptor to use. cl, cedd, fcth or jcd"),
        "hasher",
        "cl");
    parser.addOption(hasherOption);

    parser.process(app);
    const QString hasher = parser.value(hasherOption).toLower();
    const QStringList args = parser.positionalArguments();

    for (const auto& item: args)
    {
        const char format = QImage::Format_RGB32;
        const auto im = QImage(item, &format);
        if (im.isNull())
        {
            std::cout << "Can't load image: " << item.toStdString() << '\n';
            continue;
        }
        std::vector<int8_t> des;
        if (hasher == "cl")
        {
            auto cl = ColorLayoutExtractor();
            cl.extract(im);
            des = cl.get_descriptor(21, 6);
        }
        else if (hasher == "cedd")
        {
            auto cl = CEDD();
            cl.extract(im);
            des = cl.get_descriptor();
        }
        else if (hasher == "fcth")
        {
            auto cl = FCTH();
            cl.extract(im);
            des = cl.get_descriptor();
        }
        else if (hasher == "jcd")
        {
            auto cl = JCD();
            cl.extract(im);
            des = cl.get_descriptor();
        }
        else
        {
            std::cout << "Unsupported hasher." << '\n';
            std::terminate();
        }


        auto ar = QByteArray(reinterpret_cast<char*>(des.data()), des.size());

        auto j = ar.toBase64(QByteArray::Base64Encoding);

        auto s = std::string(j.data(), j.size());

        std::cout << item.toStdString() << ": " << s << '\n';
    }
    //return a.exec();
}
