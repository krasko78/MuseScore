/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2023 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "paddingtable.h"

#include "dom/engravingitem.h"
#include "style/style.h"
#include "thirdparty/kors_logger/src/log_base.h"

using namespace mu::engraving;

void PaddingTable::initPaddingTable()
{
    PaddingTable& table = *this;

    double minPadUnit = minimumPaddingUnit();

    for (size_t i=0; i < TOT_ELEMENT_TYPES; ++i) {
        for (size_t j=0; j < TOT_ELEMENT_TYPES; ++j) {
            table[i][j] = minPadUnit;
        }
    }
}

void PaddingTable::createTable(const MStyle& style)
{
    initPaddingTable();

    PaddingTable& table = *this;

    const double minPadUnit = minimumPaddingUnit();
    const double spatium = style.spatium();
    const double ledgerPad = 0.25 * spatium;
    const double ledgerLength = style.styleMM(Sid::ledgerLineLength);

    // NOTE: we don't set note-note padding to minNoteDistance
    // because there are cases when they should be allowed to get closer.
    // minNoteDistance is applied during the padding calculations.
    table[ElementType::NOTE][ElementType::NOTE] = minPadUnit;
    table[ElementType::NOTE][ElementType::LEDGER_LINE] = 0.35 * spatium;
    table[ElementType::NOTE][ElementType::ACCIDENTAL]
        = std::max(static_cast<double>(style.styleMM(Sid::accidentalNoteDistance)), 0.35 * spatium);
    table[ElementType::NOTE][ElementType::REST] = 0.5 * spatium;
    table[ElementType::NOTE][ElementType::CLEF] = 0.8 * spatium;
    table[ElementType::NOTE][ElementType::ARPEGGIO] = 0.6 * spatium;
    table[ElementType::NOTE][ElementType::BAR_LINE] = style.styleMM(Sid::noteBarDistance);
    table[ElementType::NOTE][ElementType::KEYSIG] = 0.75 * spatium;
    table[ElementType::NOTE][ElementType::TIMESIG] = 0.75 * spatium;

    // Obtain the Stem -> * and * -> Stem values from the note equivalents
    table[ElementType::STEM] = table[ElementType::NOTE];
    for (auto& elem: table) {
        elem[ElementType::STEM] = elem[ElementType::NOTE];
    }

    table[ElementType::STEM][ElementType::STEM] = 0.85 * spatium;
    table[ElementType::STEM][ElementType::ACCIDENTAL] = 0.35 * spatium;
    table[ElementType::STEM][ElementType::LEDGER_LINE] = 0.35 * spatium;
    table[ElementType::LEDGER_LINE][ElementType::STEM] = 0.35 * spatium;

    table[ElementType::LEDGER_LINE][ElementType::NOTE] = table[ElementType::NOTE][ElementType::LEDGER_LINE];
    table[ElementType::LEDGER_LINE][ElementType::LEDGER_LINE] = ledgerPad;
    table[ElementType::LEDGER_LINE][ElementType::ACCIDENTAL]
        = std::max(static_cast<double>(style.styleMM(
                                           Sid::accidentalNoteDistance)),
                   table[ElementType::NOTE][ElementType::ACCIDENTAL] - ledgerLength / 2);
    table[ElementType::LEDGER_LINE][ElementType::REST] = table[ElementType::LEDGER_LINE][ElementType::NOTE];
    table[ElementType::LEDGER_LINE][ElementType::CLEF]
        = std::max(table[ElementType::NOTE][ElementType::CLEF] - ledgerLength / 2, ledgerPad);
    table[ElementType::LEDGER_LINE][ElementType::ARPEGGIO] = 0.5 * spatium;
    table[ElementType::LEDGER_LINE][ElementType::BAR_LINE]
        = std::max(table[ElementType::NOTE][ElementType::BAR_LINE] - ledgerLength, ledgerPad);
    table[ElementType::LEDGER_LINE][ElementType::KEYSIG]
        = std::max(table[ElementType::NOTE][ElementType::KEYSIG] - ledgerLength / 2, ledgerPad);
    table[ElementType::LEDGER_LINE][ElementType::TIMESIG]
        = std::max(table[ElementType::NOTE][ElementType::TIMESIG] - ledgerLength / 2, ledgerPad);

    table[ElementType::HOOK][ElementType::NOTE] = 0.35 * spatium;
    table[ElementType::HOOK][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::HOOK][ElementType::NOTE] - ledgerLength, ledgerPad);
    table[ElementType::HOOK][ElementType::ACCIDENTAL] = 0.35 * spatium;
    table[ElementType::HOOK][ElementType::REST] = table[ElementType::HOOK][ElementType::NOTE];
    table[ElementType::HOOK][ElementType::CLEF] = 0.5 * spatium;
    table[ElementType::HOOK][ElementType::ARPEGGIO] = 0.35 * spatium;
    table[ElementType::HOOK][ElementType::BAR_LINE] = 1 * spatium;
    table[ElementType::HOOK][ElementType::KEYSIG] = 1.15 * spatium;
    table[ElementType::HOOK][ElementType::TIMESIG] = 1.15 * spatium;

    table[ElementType::NOTEDOT][ElementType::NOTE] = std::max(style.styleMM(Sid::dotNoteDistance), style.styleMM(
                                                                  Sid::dotDotDistance));
    table[ElementType::NOTEDOT][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::NOTEDOT][ElementType::NOTE] - ledgerLength, ledgerPad);
    table[ElementType::NOTEDOT][ElementType::ACCIDENTAL] = 0.35 * spatium;
    table[ElementType::NOTEDOT][ElementType::REST] = table[ElementType::NOTEDOT][ElementType::NOTE];
    table[ElementType::NOTEDOT][ElementType::CLEF] = 1.0 * spatium;
    table[ElementType::NOTEDOT][ElementType::ARPEGGIO] = 0.5 * spatium;
    table[ElementType::NOTEDOT][ElementType::BAR_LINE] = 0.8 * spatium;
    table[ElementType::NOTEDOT][ElementType::KEYSIG] = 1.35 * spatium;
    table[ElementType::NOTEDOT][ElementType::TIMESIG] = 1.35 * spatium;

    table[ElementType::REST][ElementType::NOTE] = table[ElementType::NOTE][ElementType::REST];
    table[ElementType::REST][ElementType::STEM] = table[ElementType::NOTE][ElementType::STEM];
    table[ElementType::REST][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::REST][ElementType::NOTE] - ledgerLength / 2, ledgerPad);
    table[ElementType::REST][ElementType::ACCIDENTAL] = 0.45 * spatium;
    table[ElementType::REST][ElementType::REST] = table[ElementType::REST][ElementType::NOTE];
    table[ElementType::REST][ElementType::CLEF] = table[ElementType::NOTE][ElementType::CLEF];
    table[ElementType::REST][ElementType::BAR_LINE] = 1.65 * spatium;
    table[ElementType::REST][ElementType::KEYSIG] = 1.5 * spatium;
    table[ElementType::REST][ElementType::TIMESIG] = 1.5 * spatium;

    table[ElementType::CLEF][ElementType::NOTE] = style.styleMM(Sid::clefKeyRightMargin);
    table[ElementType::CLEF][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::CLEF][ElementType::NOTE] - ledgerLength / 2, ledgerPad);
    table[ElementType::CLEF][ElementType::ACCIDENTAL] = 0.6 * spatium;
    table[ElementType::CLEF][ElementType::STEM] = 0.75 * spatium;
    table[ElementType::CLEF][ElementType::REST] = table[ElementType::CLEF][ElementType::NOTE];
    table[ElementType::CLEF][ElementType::CLEF] = 0.75 * spatium;
    table[ElementType::CLEF][ElementType::ARPEGGIO] = 0.65 * spatium;
    table[ElementType::CLEF][ElementType::BAR_LINE] = style.styleMM(Sid::clefBarlineDistance);
    table[ElementType::CLEF][ElementType::KEYSIG] = style.styleMM(Sid::clefKeyDistance);
    table[ElementType::CLEF][ElementType::TIMESIG] = style.styleMM(Sid::clefTimesigDistance);

    table[ElementType::BAR_LINE][ElementType::NOTE] = style.styleMM(Sid::barNoteDistance);
    table[ElementType::BAR_LINE][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::BAR_LINE][ElementType::NOTE] - ledgerLength, ledgerPad);
    table[ElementType::BAR_LINE][ElementType::ACCIDENTAL] = style.styleMM(Sid::barAccidentalDistance);
    table[ElementType::BAR_LINE][ElementType::REST] = style.styleMM(Sid::barNoteDistance);
    table[ElementType::BAR_LINE][ElementType::CLEF] = style.styleMM(Sid::clefLeftMargin);
    table[ElementType::BAR_LINE][ElementType::ARPEGGIO] = 0.65 * spatium;
    table[ElementType::BAR_LINE][ElementType::BAR_LINE] = 1.35 * spatium;
    table[ElementType::BAR_LINE][ElementType::KEYSIG] = style.styleMM(Sid::keysigLeftMargin);
    table[ElementType::BAR_LINE][ElementType::TIMESIG] = style.styleMM(Sid::timesigLeftMargin);

    table[ElementType::KEYSIG][ElementType::NOTE] = 1.75 * spatium;
    table[ElementType::KEYSIG][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::KEYSIG][ElementType::NOTE] - ledgerLength, ledgerPad);
    table[ElementType::KEYSIG][ElementType::ACCIDENTAL] = 1.6 * spatium;
    table[ElementType::KEYSIG][ElementType::REST] = table[ElementType::KEYSIG][ElementType::NOTE];
    table[ElementType::KEYSIG][ElementType::CLEF] = 1.0 * spatium;
    table[ElementType::KEYSIG][ElementType::ARPEGGIO] = 1.35 * spatium;
    table[ElementType::KEYSIG][ElementType::BAR_LINE] = style.styleMM(Sid::keyBarlineDistance);
    table[ElementType::KEYSIG][ElementType::KEYSIG] = 1 * spatium;
    table[ElementType::KEYSIG][ElementType::TIMESIG] = style.styleMM(Sid::keyTimesigDistance);

    table[ElementType::TIMESIG][ElementType::NOTE] = 1.35 * spatium;
    table[ElementType::TIMESIG][ElementType::LEDGER_LINE]
        = std::max(table[ElementType::TIMESIG][ElementType::NOTE] - ledgerLength, ledgerPad);
    table[ElementType::TIMESIG][ElementType::ACCIDENTAL] = 0.8 * spatium;
    table[ElementType::TIMESIG][ElementType::REST] = table[ElementType::TIMESIG][ElementType::NOTE];
    table[ElementType::TIMESIG][ElementType::CLEF] = 1.0 * spatium;
    table[ElementType::TIMESIG][ElementType::ARPEGGIO] = 1.35 * spatium;
    table[ElementType::TIMESIG][ElementType::BAR_LINE] = style.styleMM(Sid::timesigBarlineDistance);
    table[ElementType::TIMESIG][ElementType::KEYSIG] = style.styleMM(Sid::keyTimesigDistance);
    table[ElementType::TIMESIG][ElementType::TIMESIG] = 1.0 * spatium;

    // Ambitus
    table[ElementType::AMBITUS].fill(style.styleMM(Sid::ambitusMargin));
    for (auto& elem: table) {
        elem[ElementType::AMBITUS] = style.styleMM(Sid::ambitusMargin);
    }

    table[ElementType::ARPEGGIO][ElementType::NOTE] = style.styleMM(Sid::arpeggioNoteDistance);
    table[ElementType::ARPEGGIO][ElementType::LEDGER_LINE] = 0.3 * spatium;
    table[ElementType::ARPEGGIO][ElementType::ACCIDENTAL] = style.styleMM(Sid::arpeggioAccidentalDistance);

    // Breath
    table[ElementType::BREATH].fill(1.0 * spatium);
    for (auto& elem: table) {
        elem[ElementType::BREATH] = 1.0 * spatium;
    }

    // Harmony
    table[ElementType::BAR_LINE][ElementType::HARMONY] = 0.5 * style.styleMM(Sid::minHarmonyDistance);
    table[ElementType::HARMONY][ElementType::HARMONY] = style.styleMM(Sid::minHarmonyDistance);
    table[ElementType::HARMONY][ElementType::FRET_DIAGRAM] = 0.3 * spatium;
    table[ElementType::FRET_DIAGRAM][ElementType::HARMONY] = 0.3 * spatium;
    table[ElementType::FRET_DIAGRAM][ElementType::FRET_DIAGRAM] = 0.25 * spatium;

    // Chordlines
    table[ElementType::CHORDLINE].fill(0.35 * spatium);
    for (auto& elem: table) {
        elem[ElementType::CHORDLINE] = 0.35 * spatium;
    }
    table[ElementType::BAR_LINE][ElementType::CHORDLINE] = 0.65 * spatium;
    table[ElementType::CHORDLINE][ElementType::BAR_LINE] = 0.65 * spatium;

    // For the x -> fingering padding use the same values as x -> accidental
    for (auto& elem : table) {
        elem[ElementType::FINGERING] = elem[ElementType::ACCIDENTAL];
    }

    // This is needed for beamlets, not beams themselves
    table[ElementType::BEAM].fill(0.35 * spatium);

    table[ElementType::TREMOLO_SINGLECHORD] = table[ElementType::BEAM];

    double lyricsSpacing = style.styleMM(Sid::lyricsMinDistance);
    table[ElementType::LYRICS].fill(lyricsSpacing);
    for (auto& elem : table) {
        elem[ElementType::LYRICS] = lyricsSpacing;
    }
    table[ElementType::NOTE][ElementType::LYRICS] = style.styleMM(Sid::lyricsMelismaPad);

    // Accidental -> padding (used by accidental placement algorithm)
    table[ElementType::ACCIDENTAL][ElementType::NOTE] = style.styleMM(Sid::accidentalNoteDistance);
    table[ElementType::ACCIDENTAL][ElementType::LEDGER_LINE] = 0.18 * spatium;
    table[ElementType::ACCIDENTAL][ElementType::STEM] = table[ElementType::ACCIDENTAL][ElementType::NOTE];

    table[ElementType::ARTICULATION][ElementType::NOTE] = 0.25 * spatium;
    table[ElementType::ARTICULATION][ElementType::REST] = 0.25 * spatium;
    table[ElementType::ARTICULATION][ElementType::ACCIDENTAL] = 0.25 * spatium;

    table[ElementType::LAISSEZ_VIB_SEGMENT][ElementType::NOTE] = 0.5 * spatium;
    table[ElementType::LAISSEZ_VIB_SEGMENT][ElementType::REST] = 0.5 * spatium;
    table[ElementType::LAISSEZ_VIB_SEGMENT][ElementType::ACCIDENTAL] = 0.35 * spatium;
    table[ElementType::LAISSEZ_VIB_SEGMENT][ElementType::BAR_LINE] = 0.35 * spatium;
    table[ElementType::LAISSEZ_VIB_SEGMENT][ElementType::STEM] = 0.35 * spatium;

    table[ElementType::PARENTHESIS][ElementType::PARENTHESIS] = 1.0 * spatium;

    // Measure repeat set same values as note
    table[ElementType::MEASURE_REPEAT] = table[ElementType::NOTE];
    for (auto& elem : table) {
        elem[ElementType::MEASURE_REPEAT] = elem[ElementType::NOTE];
    }

    const double articulationAndFermataPadding = 0.35 * spatium;
    table[ElementType::ARTICULATION].fill(articulationAndFermataPadding);
    for (auto& elem : table) {
        elem[ElementType::ARTICULATION] = articulationAndFermataPadding;
    }
    table[ElementType::FERMATA].fill(articulationAndFermataPadding);
    for (auto& elem : table) {
        elem[ElementType::FERMATA] = articulationAndFermataPadding;
    }

    table[ElementType::TAPPING] = table[ElementType::ARTICULATION];
    for (auto& elem : table) {
        elem[ElementType::TAPPING] = elem[ElementType::ARTICULATION];
    }
    table[ElementType::TAPPING_HALF_SLUR_SEGMENT] = table[ElementType::ARTICULATION];
    for (auto& elem : table) {
        elem[ElementType::TAPPING_HALF_SLUR_SEGMENT] = elem[ElementType::ARTICULATION];
    }
}

double ParenPaddingTable::padding(ElementType type1, ElementType type2)
{
    assert(type1 == ElementType::PARENTHESIS || type2 == ElementType::PARENTHESIS);
    const PaddingVector<double>& row = type1 == ElementType::PARENTHESIS ? m_parenBefore : m_parenAfter;
    const ElementType& otherType = type1 == ElementType::PARENTHESIS ? type2 : type1;

    return row.at(otherType);
}

ParenPaddingTablePtr ParenPaddingTable::getPaddingTable(const EngravingItem* parent)
{
    ParenPaddingTablePtr table;
    switch (parent->type()) {
    case ElementType::NOTE:
        table = std::make_unique<NoteParenPaddingTable>();
        break;
    case ElementType::KEYSIG:
        table = std::make_unique<KeySigParenPaddingTable>();
        break;
    case ElementType::TIMESIG:
        table = std::make_unique<TimeSigParenPaddingTable>();
        break;
    case ElementType::CLEF:
        table = std::make_unique<ClefParenPaddingTable>();
        break;
    default:
        ASSERT_X("Not a valid parenthesised type")
    }

    table->setMinimumPaddingUnit(0.1 * parent->style().spatium());
    table->createTable(parent->style());

    return table;
}

void ParenPaddingTable::initPaddingTable()
{
    const double minPadUnit = minimumPaddingUnit();

    for (size_t i = 0; i < TOT_ELEMENT_TYPES; ++i) {
        m_parenBefore[i] = minPadUnit;
        m_parenAfter[i]  = minPadUnit;
    }
}

void NoteParenPaddingTable::createTable(const MStyle& style)
{
    const double spatium = style.spatium();

    m_parenBefore[ElementType::ARPEGGIO] = style.styleMM(Sid::arpeggioAccidentalDistance);
    m_parenBefore[ElementType::BAR_LINE] = style.styleMM(Sid::barAccidentalDistance);
    m_parenBefore[ElementType::CLEF] = 0.6 * spatium;
    m_parenBefore[ElementType::HOOK] = 0.35 * spatium;
    m_parenBefore[ElementType::KEYSIG] = 1.6 * spatium;
    m_parenBefore[ElementType::LEDGER_LINE] = style.styleMM(Sid::accidentalNoteDistance).val();
    m_parenBefore[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenBefore[ElementType::NOTE] = 0.35 * spatium;
    m_parenBefore[ElementType::REST] = 0.45 * spatium;
    m_parenBefore[ElementType::STEM] = 0.35 * spatium;
    m_parenBefore[ElementType::TIMESIG] = 0.8 * spatium;

    m_parenAfter[ElementType::ACCIDENTAL] = std::max(style.styleMM(Sid::accidentalNoteDistance).val(), 0.35 * spatium);
    m_parenAfter[ElementType::ARPEGGIO] = 0.6 * spatium;
    m_parenAfter[ElementType::BAR_LINE] = style.styleMM(Sid::noteBarDistance);
    m_parenAfter[ElementType::CLEF] = 0.8 * spatium;
    m_parenAfter[ElementType::KEYSIG] = 0.75 * spatium;
    m_parenAfter[ElementType::LEDGER_LINE] = 0.35 * spatium;
    m_parenAfter[ElementType::NOTE] = std::max(style.styleMM(Sid::accidentalNoteDistance).val(), 0.35 * spatium);
    m_parenAfter[ElementType::REST] = style.styleMM(Sid::minNoteDistance);
    m_parenAfter[ElementType::STEM] = style.styleMM(Sid::minNoteDistance);
    m_parenAfter[ElementType::TIMESIG] = 0.75 * spatium;
}

void KeySigParenPaddingTable::createTable(const MStyle& style)
{
    const double spatium = style.spatium();

    m_parenBefore[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenBefore[ElementType::CLEF] = 0.25 * spatium;
    m_parenBefore[ElementType::HOOK] = 0.35 * spatium;
    m_parenBefore[ElementType::KEYSIG] = 0.25 * spatium;
    m_parenBefore[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenBefore[ElementType::NOTE] = style.styleMM(Sid::noteBarDistance);
    m_parenBefore[ElementType::REST] = m_parenBefore[ElementType::NOTE];
    m_parenBefore[ElementType::STEM] = 0.35 * spatium;
    m_parenBefore[ElementType::TIMESIG] = 0.25 * spatium;

    m_parenAfter[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenAfter[ElementType::CLEF] = 0.2 * spatium;
    m_parenAfter[ElementType::HOOK] = 0.35 * spatium;
    m_parenAfter[ElementType::KEYSIG] = 0.25 * spatium;
    m_parenAfter[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenAfter[ElementType::NOTE] = style.styleMM(Sid::barNoteDistance);
    m_parenAfter[ElementType::REST] = m_parenAfter[ElementType::NOTE];
    m_parenAfter[ElementType::STEM] = 0.35 * spatium;
    m_parenAfter[ElementType::TIMESIG] = 0.25 * spatium;
}

void TimeSigParenPaddingTable::createTable(const MStyle& style)
{
    const double spatium = style.spatium();

    m_parenBefore[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenBefore[ElementType::CLEF] = 0.25 * spatium;
    m_parenBefore[ElementType::HOOK] = 0.35 * spatium;
    m_parenBefore[ElementType::KEYSIG] = 0.25 * spatium;
    m_parenBefore[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenBefore[ElementType::NOTE] = style.styleMM(Sid::noteBarDistance);
    m_parenBefore[ElementType::REST] = m_parenBefore[ElementType::NOTE];
    m_parenBefore[ElementType::STEM] = 0.35 * spatium;
    m_parenBefore[ElementType::TIMESIG] = 0.25 * spatium;

    m_parenAfter[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenAfter[ElementType::CLEF] = 0.2 * spatium;
    m_parenAfter[ElementType::HOOK] = 0.35 * spatium;
    m_parenAfter[ElementType::KEYSIG] = 0.35 * spatium;
    m_parenAfter[ElementType::NOTEDOT] = 0.2 * spatium;
    m_parenAfter[ElementType::NOTE] = style.styleMM(Sid::barNoteDistance);
    m_parenAfter[ElementType::REST] = m_parenAfter[ElementType::NOTE];
    m_parenAfter[ElementType::STEM] = 0.35 * spatium;
    m_parenAfter[ElementType::TIMESIG] = 0.25 * spatium;
}

void ClefParenPaddingTable::createTable(const MStyle& style)
{
    const double spatium = style.spatium();

    m_parenBefore[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenBefore[ElementType::CLEF] = 0.25 * spatium;
    m_parenBefore[ElementType::HOOK] = 0.35 * spatium;
    m_parenBefore[ElementType::KEYSIG] = 0.25 * spatium;
    m_parenBefore[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenBefore[ElementType::NOTE] = style.styleMM(Sid::noteBarDistance);
    m_parenBefore[ElementType::REST] = m_parenBefore[ElementType::NOTE];
    m_parenBefore[ElementType::STEM] = 0.35 * spatium;
    m_parenBefore[ElementType::TIMESIG] = 0.25 * spatium;

    m_parenAfter[ElementType::BAR_LINE] = 0.5 * spatium;
    m_parenAfter[ElementType::CLEF] = 0.25 * spatium;
    m_parenAfter[ElementType::HOOK] = 0.35 * spatium;
    m_parenAfter[ElementType::KEYSIG] = 0.35 * spatium;
    m_parenAfter[ElementType::NOTEDOT] = 0.35 * spatium;
    m_parenAfter[ElementType::NOTE] = style.styleMM(Sid::barNoteDistance);
    m_parenAfter[ElementType::REST] = m_parenAfter[ElementType::NOTE];
    m_parenAfter[ElementType::STEM] = 0.35 * spatium;
    m_parenAfter[ElementType::TIMESIG] = 0.2 * spatium;
}
