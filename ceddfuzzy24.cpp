// ceddfuzzy24.cpp
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

#include <ceddfuzzy24.h>


namespace {

constexpr std::array<double, 8> SaturationMembershipValues = { 0,  0,   68,  188,
                                                               68, 188, 255, 255 };
constexpr std::array<double, 8> ValueMembershipValues = { 0,  0,   68,  188,
                                                          68, 188, 255, 255 };
constexpr int rules_length = 4;
constexpr FuzzyRules Fuzzy24BinRules[rules_length] = {
    { 1, 1, 1, 0 }, { 0, 0, 2, 0 }, { 0, 1, 0, 0 }, { 1, 0, 2, 0 }
};

} // namespace

std::vector<double> CeddFuzzy24::ApplyFilter(double Saturation,
                                             double Value,
                                             const std::vector<double>& ColorValues,
                                             int Method)
{
    // Method   0 = LOM
    //          1 = Multi Equal Participate
    //          2 = Multi Participate

    resultsTable.fill(0.0);
    double Temp = 0;

    std::array<double, 2> SaturationActivation{};
    std::array<double, 2> ValueActivation{};

    FindMembershipValueForTriangles(Saturation, SaturationMembershipValues, &SaturationActivation);
    FindMembershipValueForTriangles(Value, ValueMembershipValues, &ValueActivation);

    if (!keep_values) { fuzzy24BinHisto.fill(0.0); }

    for (int i = 3; i < 10; i++)
    {
        Temp += ColorValues[i];
    }

    if (Temp > 0)
    {
        if (Method == 0)
            LOM_Defazzificator(SaturationActivation, ValueActivation);
        if (Method == 1)
            MultiParticipate_Equal_Defazzificator(SaturationActivation, ValueActivation);
        if (Method == 2)
            MultiParticipate_Defazzificator(SaturationActivation, ValueActivation);
    }

    for (int i = 0; i < 3; i++)
    {
        fuzzy24BinHisto[i] += ColorValues[i];
    }


    for (int i = 3; i < 10; i++)
    {
        fuzzy24BinHisto[(i - 2) * 3] += ColorValues[i] * resultsTable[0];
        fuzzy24BinHisto[(i - 2) * 3 + 1] += ColorValues[i] * resultsTable[1];
        fuzzy24BinHisto[(i - 2) * 3 + 2] += ColorValues[i] * resultsTable[2];
    }

    return { fuzzy24BinHisto.cbegin(), fuzzy24BinHisto.cend() };
}

void CeddFuzzy24::FindMembershipValueForTriangles(const double Input,
                                                  const std::array<double, 8> &Triangles,
                                                  std::array<double, 2> *save_to)
{
    int Temp = 0;

    for (auto i = 0u; i <= Triangles.size() - 1; i += 4)
    {
        (*save_to)[Temp] = 0;

        if (Input >= Triangles[i + 1] && Input <= +Triangles[i + 2])
        {
            (*save_to)[Temp] = 1;
        }
        if (Input >= Triangles[i] && Input < Triangles[i + 1])
        {
            (*save_to)[Temp] = (Input - Triangles[i]) / (Triangles[i + 1] - Triangles[i]);
        }
        if (Input > Triangles[i + 2] && Input <= Triangles[i + 3])
        {
            (*save_to)[Temp] = (Input - Triangles[i + 2]) / (Triangles[i + 2] - Triangles[i + 3]) + 1;
        }

        Temp += 1;
    }
}

void CeddFuzzy24::LOM_Defazzificator(const std::array<double, 2> &Input1,
                                     const std::array<double, 2> &Input2)
{
    int RuleActivation = -1;
    double LOM_MAXofMIN = 0;

    for (int i = 0; i < rules_length; i++)
    {

        if ((Input1[Fuzzy24BinRules[i].Input1] > 0) && (Input2[Fuzzy24BinRules[i].Input2] > 0))
        {
            double Min = std::min(Input1[Fuzzy24BinRules[i].Input1], Input2[Fuzzy24BinRules[i].Input2]);

            if (Min > LOM_MAXofMIN)
            {
                LOM_MAXofMIN = Min;
                RuleActivation = Fuzzy24BinRules[i].Output;
            }
        }
    }
    resultsTable[RuleActivation]++;
}

void CeddFuzzy24::MultiParticipate_Equal_Defazzificator(const std::array<double, 2> &Input1,
                                                        const std::array<double, 2> &Input2)
{
    int RuleActivation = -1;

    for (int i = 0; i < rules_length; i++)
    {
        if ((Input1[Fuzzy24BinRules[i].Input1] > 0) &&
            (Input2[Fuzzy24BinRules[i].Input2] > 0))
        {
            RuleActivation = Fuzzy24BinRules[i].Output;
            resultsTable[RuleActivation]++;
        }
    }
}


void CeddFuzzy24::MultiParticipate_Defazzificator(const std::array<double, 2> &Input1,
                                                  const std::array<double, 2> &Input2)
{
    int RuleActivation = -1;
    double Min = 0;
    for (int i = 0; i < rules_length; i++)
    {
        if ((Input1[Fuzzy24BinRules[i].Input1] > 0) &&
            (Input2[Fuzzy24BinRules[i].Input2] > 0))
        {
            Min = std::min(Input1[Fuzzy24BinRules[i].Input1], Input2[Fuzzy24BinRules[i].Input2]);

            RuleActivation = Fuzzy24BinRules[i].Output;
            resultsTable[RuleActivation] += Min;
        }
    }
}


CeddFuzzy24::CeddFuzzy24(bool a_keep_values)
    : keep_values(a_keep_values)
{
}
