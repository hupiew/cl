// ceddfuzzy10.cpp
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

#include <ceddfuzzy10.h>


namespace {

constexpr uint16_t hueMembershipValues[32]{ 0,   0,   5,   10,  5,   10,  35,  50,
                                            35,  50,  70,  85,  70,  85,  150, 165,
                                            150, 165, 195, 205, 195, 205, 265, 280,
                                            265, 280, 315, 330, 315, 330, 360, 360 };

constexpr uint16_t saturationMembershipValues[8]{ 0, 0, 10, 75, 10, 75, 255, 255 };

constexpr uint16_t valueMembershipValues[12]{ 0,   0,   10,  75,  10,  75,
                                              180, 220, 180, 220, 255, 255 };

constexpr int rules_length = 48;
constexpr FuzzyRules Fuzzy10BinRules[rules_length] = {
    { 0, 0, 0, 2 }, { 0, 1, 0, 2 }, { 0, 0, 2, 0 }, { 0, 0, 1, 1 }, { 1, 0, 0, 2 },
    { 1, 1, 0, 2 }, { 1, 0, 2, 0 }, { 1, 0, 1, 1 }, { 2, 0, 0, 2 }, { 2, 1, 0, 2 },
    { 2, 0, 2, 0 }, { 2, 0, 1, 1 }, { 3, 0, 0, 2 }, { 3, 1, 0, 2 }, { 3, 0, 2, 0 },
    { 3, 0, 1, 1 }, { 4, 0, 0, 2 }, { 4, 1, 0, 2 }, { 4, 0, 2, 0 }, { 4, 0, 1, 1 },
    { 5, 0, 0, 2 }, { 5, 1, 0, 2 }, { 5, 0, 2, 0 }, { 5, 0, 1, 1 }, { 6, 0, 0, 2 },
    { 6, 1, 0, 2 }, { 6, 0, 2, 0 }, { 6, 0, 1, 1 }, { 7, 0, 0, 2 }, { 7, 1, 0, 2 },
    { 7, 0, 2, 0 }, { 7, 0, 1, 1 }, { 0, 1, 1, 3 }, { 0, 1, 2, 3 }, { 1, 1, 1, 4 },
    { 1, 1, 2, 4 }, { 2, 1, 1, 5 }, { 2, 1, 2, 5 }, { 3, 1, 1, 6 }, { 3, 1, 2, 6 },
    { 4, 1, 1, 7 }, { 4, 1, 2, 7 }, { 5, 1, 1, 8 }, { 5, 1, 2, 8 }, { 6, 1, 1, 9 },
    { 6, 1, 2, 9 }, { 7, 1, 1, 3 }, { 7, 1, 2, 3 }
};

} // namespace


CeddFuzzy10::CeddFuzzy10(bool keep_values)
    : keep_values(keep_values) {}


std::vector<double> CeddFuzzy10::apply_filter(double hue,
                                              double saturation,
                                              double value,
                                              int method)
{
    if (!keep_values) { fuzzy10BinHisto.fill(0.0); }

    std::vector<double> hueActivation(8);
    std::vector<double> saturationActivation(8);
    std::vector<double> valueActivation(8);

    findMembershipValueForTriangles(hue, hueMembershipValues, 32, &hueActivation);
    findMembershipValueForTriangles(
        saturation, saturationMembershipValues, 8, &saturationActivation);
    findMembershipValueForTriangles(value, valueMembershipValues, 12, &valueActivation);

    if (method == 0)
        LOM_Defazzificator(Fuzzy10BinRules,
                           rules_length,
                           hueActivation,
                           saturationActivation,
                           valueActivation,
                           &fuzzy10BinHisto);
    else if (method == 1)
        MultiParticipate_Equal_Defazzificator(Fuzzy10BinRules,
                                              rules_length,
                                              hueActivation,
                                              saturationActivation,
                                              valueActivation,
                                              &fuzzy10BinHisto);
    else if (method == 2)
        MultiParticipate_Defazzificator(Fuzzy10BinRules,
                                        rules_length,
                                        hueActivation,
                                        saturationActivation,
                                        valueActivation,
                                        &fuzzy10BinHisto);

    return { fuzzy10BinHisto.cbegin(), fuzzy10BinHisto.cend() };
}

void CeddFuzzy10::findMembershipValueForTriangles(const double input,
                                                  const uint16_t* triangles,
                                                  const int size,
                                                  std::vector<double> *to_save)
{
    int Temp = 0;

    for (int i = 0; i <= size - 1; i += 4)
    {
        (*to_save)[Temp] = 0;

        if (input >= triangles[i + 1] && input <= +triangles[i + 2])
        {
            (*to_save)[Temp] = 1;
        }

        if (input >= triangles[i] && input < triangles[i + 1])
        {
            (*to_save)[Temp] = (input - triangles[i]) / (triangles[i + 1] - triangles[i]);
        }

        if (input > triangles[i + 2] && input <= triangles[i + 3])
        {
            (*to_save)[Temp] = (input - triangles[i + 2]) / (triangles[i + 2] - triangles[i + 3]) + 1;
        }

        Temp += 1;
    }
}

void CeddFuzzy10::LOM_Defazzificator(const FuzzyRules Rules[],
                                     int rules_len,
                                     const std::vector<double>& Input1,
                                     const std::vector<double>& Input2,
                                     const std::vector<double>& Input3,
                                     std::array<double, 10>* ResultTable)
{
    int RuleActivation = -1;
    double LOM_MAXofMIN = 0;

    for (int i = 0; i < rules_len; i++)
    {

        if ((Input1[Rules[i].Input1] > 0) &&
            (Input2[Rules[i].Input2] > 0) &&
            (Input3[Rules[i].Input3] > 0))
        {
            double Min = std::min(Input1[Rules[i].Input1],
                    std::min(Input2[Rules[i].Input2], Input3[Rules[i].Input3]));

            if (Min > LOM_MAXofMIN)
            {
                LOM_MAXofMIN = Min;
                RuleActivation = Rules[i].Output;
            }
        }
    }
    (*ResultTable)[RuleActivation]++;
}

void CeddFuzzy10::MultiParticipate_Equal_Defazzificator(
    const FuzzyRules Rules[],
    int rules_len,
    const std::vector<double>& Input1,
    const std::vector<double>& Input2,
    const std::vector<double>& Input3,
    std::array<double, 10>* ResultTable)
{
    for (int i = 0; i < rules_len; i++)
    {
        if ((Input1[Rules[i].Input1] > 0) &&
            (Input2[Rules[i].Input2] > 0) &&
            (Input3[Rules[i].Input3] > 0))
        {
            const auto RuleActivation = Rules[i].Output;
            (*ResultTable)[RuleActivation]++;
        }
    }
}

void CeddFuzzy10::MultiParticipate_Defazzificator(const FuzzyRules Rules[],
                                                  int rules_len,
                                                  const std::vector<double>& Input1,
                                                  const std::vector<double>& Input2,
                                                  const std::vector<double>& Input3,
                                                  std::array<double, 10>* ResultTable)
{
    for (int i = 0; i < rules_len; i++)
    {
        if ((Input1[Rules[i].Input1] > 0) &&
            (Input2[Rules[i].Input2] > 0) &&
            (Input3[Rules[i].Input3] > 0))
        {
            const auto RuleActivation = Rules[i].Output;

            const auto Min =
                std::min(Input1[Rules[i].Input1],
                         std::min(Input2[Rules[i].Input2], Input3[Rules[i].Input3]));

            (*ResultTable)[RuleActivation] += Min;
        }
    }
}
