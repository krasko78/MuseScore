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
#include "tupletlayout.h"

#include "dom/chordrest.h"
#include "dom/tuplet.h"
#include "dom/text.h"
#include "dom/factory.h"
#include "dom/stafftype.h"
#include "dom/chord.h"
#include "dom/note.h"
#include "dom/stem.h"
#include "dom/system.h"
#include "dom/measure.h"

#include "tlayout.h"
#include "autoplace.h"

using namespace mu::engraving;
using namespace mu::engraving::rendering::score;

void TupletLayout::layout(Tuplet* item, LayoutContext& ctx)
{
    Tuplet::LayoutData* ldata = item->mutldata();
    if (item->elements().empty()) {
        LOGD("Tuplet::layout(): tuplet is empty");
        return;
    }

    const StaffType* stt = item->staffType();
    if (stt && stt->isTabStaff() && stt->stemless()) {
        // is in a TAB without stems, skip any format: tuplets are not shown
        return;
    }

    createNumber(item, ctx);

    computeDirection(item);

    const ChordRest* cr1 = nullptr;
    const ChordRest* cr2 = nullptr;
    computeStartEndCR(item, &cr1, &cr2);

    bool hasBracket = item->calcHasBracket(cr1, cr2);
    item->setHasBracket(hasBracket);

    double mag = (cr1->mag() + cr2->mag()) / 2;
    ldata->setMag(mag);

    item->setPos(0.0, 0.0);

    layoutBracket(item, cr1, cr2, ctx);

    // collect bounding box
    RectF r;
    if (item->number()) {
        r |= item->number()->ldata()->bbox().translated(item->number()->pos());
        if (item->hasBracket()) {
            RectF b;
            b.setCoords(item->bracketL[1].x(), item->bracketL[1].y(), item->bracketR[2].x(), item->bracketR[2].y());
            r |= b;
        }
    } else if (item->hasBracket()) {
        RectF b;
        b.setCoords(item->bracketL[1].x(), item->bracketL[1].y(), item->bracketL[3].x(), item->bracketL[3].y());
        r |= b;
    }
    item->setbbox(r);

    if (!item->cross()) {
        Autoplace::autoplaceMeasureElement(item, item->mutldata(), item->isUp(), /* add to skyline */ true);
    }
}

void TupletLayout::layoutTupletAndNestedTuplets(Tuplet* t, LayoutContext& ctx)
{
    const std::vector<DurationElement*> elements = t->elements();
    for (auto revIter = elements.rbegin(); revIter != elements.rend(); ++revIter) {
        DurationElement* d = *revIter;
        if (d == t) {
            continue;
        }
        // if element is tuplet, layoutTuplet(that tuplet)
        if (d->isTuplet()) {
            layoutTupletAndNestedTuplets(toTuplet(d), ctx);
        }
    }
    // layout t
    layout(t, ctx);
}

bool TupletLayout::isTopTuplet(ChordRest* cr)
{
    Tuplet* t = cr->tuplet();
    if (t && t->elements().front() == cr) {
        // find top level tuplet
        while (t->tuplet()) {
            t = t->tuplet();
        }
        // consider tuplet cross if anything moved within it
        if (t->cross()) {
            return false;
        } else {
            return true;
        }
    }

    // no tuplet or not first element
    return false;
}

bool TupletLayout::notTopTuplet(ChordRest* cr)
{
    Tuplet* t = cr->tuplet();
    if (t && t->elements().front() == cr) {
        // find top level tuplet
        while (t->tuplet()) {
            t = t->tuplet();
        }
        // consider tuplet cross if anything moved within it
        if (t->cross()) {
            return true;
        } else {
            return false;
        }
    }

    // no tuplet or not first element
    return false;
}

void TupletLayout::createNumber(Tuplet* item, LayoutContext& ctx)
{
    if (item->numberType() == TupletNumberType::NO_TEXT) {
        if (item->number()) {
            if (item->number()->selected()) {
                ctx.deselect(item->number());
            }
            delete item->number();
            item->setNumber(nullptr);
        }

        return;
    }

    if (!item->number()) {
        Text* number = Factory::createText(item, TextStyleType::TUPLET);
        number->setComposition(true);
        number->setTrack(item->track());
        number->setParent(item);
        number->setVisible(item->visible());
        number->setColor(item->color());
        item->setNumber(number);
        item->resetNumberProperty();
    }

    // tuplet properties are propagated to number automatically by setProperty()
    // but we need to make sure flags are as well
    item->number()->setPropertyFlags(Pid::FONT_FACE, item->propertyFlags(Pid::FONT_FACE));
    item->number()->setPropertyFlags(Pid::FONT_SIZE, item->propertyFlags(Pid::FONT_SIZE));
    item->number()->setPropertyFlags(Pid::FONT_STYLE, item->propertyFlags(Pid::FONT_STYLE));
    item->number()->setPropertyFlags(Pid::ALIGN, item->propertyFlags(Pid::ALIGN));

    const MStyle& style = ctx.conf().style();

    String numberString = (item->numberType() == TupletNumberType::SHOW_NUMBER)
                          ? String(u"%1").arg(item->ratio().numerator())
                          : String(u"%1:%2").arg(item->ratio().numerator(), item->ratio().denominator());
    if (style.styleB(Sid::tupletUseSymbols)) {
        String smuflNum;
        for (size_t i = 0; i < numberString.size(); ++i) {
            smuflNum.append(u"<sym>tuplet");
            smuflNum.append(numberString.at(i).unicode());
            smuflNum.append(u"</sym>");
        }
        smuflNum.replace(String(u":"), String(u"Colon"));
        item->number()->setXmlText(smuflNum);
    } else {
        item->number()->setXmlText(numberString);
    }

    item->setIsSmall(true);
    for (const DurationElement* e : item->elements()) {
        if ((e->isChordRest() && !toChordRest(e)->isSmall()) || (e->isTuplet() && !toTuplet(e)->isSmall())) {
            item->setIsSmall(false);
            break;
        }
    }

    item->number()->mutldata()->setMag(item->isSmall() ? style.styleD(Sid::smallNoteMag) : 1.0);
}

void TupletLayout::computeDirection(Tuplet* item)
{
    if (item->direction() != DirectionV::AUTO) {
        item->setIsUp(item->direction() == DirectionV::UP);
        return;
    }

    int up = 0;
    for (const DurationElement* e : item->elements()) {
        if (e->isChord()) {
            const Chord* c = toChord(e);
            if (c->stemDirection() != DirectionV::AUTO) {
                up += c->stemDirection() == DirectionV::UP ? 1000 : -1000;
            } else {
                up += c->up() ? 1 : -1;
            }
        }
    }
    if (up == 0) {
        // this is a tuplet full of rests, default to up but also take voices into consideration
        Measure* m = item->measure();
        if (m && m->hasVoices(item->staffIdx())) {
            up = item->voice() % 2 == 0 ? 1 : -1;
        } else {
            up = 1;             // default up
        }
    }

    item->setIsUp(up > 0);
}

void TupletLayout::computeStartEndCR(Tuplet* item, const ChordRest** cr1, const ChordRest** cr2)
{
    const DurationElement* startEl = item->elements().front();
    while (startEl->isTuplet()) {
        const Tuplet* t = toTuplet(startEl);
        if (t->elements().empty()) {
            break;
        }
        startEl = t->elements().front();
    }
    assert(startEl && startEl->isChordRest());

    const DurationElement* endEl = item->elements().back();
    while (endEl->isTuplet()) {
        const Tuplet* t = toTuplet(endEl);
        if (t->elements().empty()) {
            break;
        }
        endEl = t->elements().back();
    }
    assert(endEl && endEl->isChordRest());

    *cr1 = toChordRest(startEl);
    *cr2 = toChordRest(endEl);
}

void TupletLayout::layoutBracket(Tuplet* item, const ChordRest* cr1, const ChordRest* cr2, LayoutContext& ctx)
{
    const MStyle& style = ctx.conf().style();
    double spatium = item->spatium();

    double maxSlope      = style.styleD(Sid::tupletMaxSlope);
    bool outOfStaff      = style.styleB(Sid::tupletOutOfStaff);
    double vHeadDistance = style.styleMM(Sid::tupletVHeadDistance) * item->mag();
    double vStemDistance = style.styleMM(Sid::tupletVStemDistance) * item->mag();
    double stemLeft      = (style.styleMM(Sid::tupletStemLeftDistance) - style.styleMM(Sid::tupletBracketWidth) / 2) * cr1->mag();
    double stemRight     = (style.styleMM(Sid::tupletStemRightDistance) - style.styleMM(Sid::tupletBracketWidth) / 2) * cr2->mag();
    double noteLeft      = (style.styleMM(Sid::tupletNoteLeftDistance) - style.styleMM(Sid::tupletBracketWidth) / 2) * cr1->mag();
    double noteRight     = (style.styleMM(Sid::tupletNoteRightDistance) - style.styleMM(Sid::tupletBracketWidth) / 2) * cr2->mag();

    int move = 0;
    if (outOfStaff && cr1->isChordRest() && cr2->isChordRest()) {
        // account for staff move when adjusting bracket to avoid staff
        // but don't attempt adjustment unless both endpoints are in same staff
        if (toChordRest(cr1)->staffMove() == toChordRest(cr2)->staffMove()) {
            move = toChordRest(cr1)->staffMove();
        } else {
            outOfStaff = false;
        }
    }

    double l1  = style.styleMM(Sid::tupletBracketHookHeight) * item->mag();
    double l2l = vHeadDistance;      // left bracket vertical distance
    double l2r = vHeadDistance;      // right bracket vertical distance

    if (item->isUp()) {
        vHeadDistance = -vHeadDistance;
    }

    item->setP1(cr1->pagePos());
    item->setP2(cr2->pagePos());

    item->p1().rx() -= noteLeft;
    item->p2().rx() += ctx.conf().noteHeadWidth() + noteRight;
    item->p1().ry() += vHeadDistance;          // TODO: Direction ?
    item->p2().ry() += vHeadDistance;

    double xx1 = item->p1().x();   // use to center the number on the beam

    double leftNoteEdge = 0.0; // page coordinates
    double rightNoteEdge = 0.0;
    if (cr1->isChord()) {
        const Chord* chord1 = toChord(cr1);
        leftNoteEdge = chord1->up() ? chord1->downNote()->pageBoundingRect().left() : chord1->upNote()->pageBoundingRect().left();
    }
    if (cr2->isChord()) {
        const Chord* chord2 = toChord(cr2);
        rightNoteEdge = chord2->up() ? chord2->downNote()->pageBoundingRect().right() : chord2->upNote()->pageBoundingRect().right();
    }

    if (item->isUp()) {
        if (cr1->isChord()) {
            const Chord* chord1 = toChord(cr1);
            Stem* stem = chord1->stem();
            if (stem) {
                xx1 = stem->pageBoundingRect().x();
            }
            if (chord1->up() && stem) {
                item->p1().ry() = stem->pageBoundingRect().y();
                l2l = vStemDistance;
                item->p1().rx() = stem->pageBoundingRect().left() - stemLeft;
            } else {
                item->p1().ry() = chord1->upNote()->pageBoundingRect().top();
                item->p1().rx() = leftNoteEdge - noteLeft;
            }
        }

        if (cr2->isChord()) {
            const Chord* chord2 = toChord(cr2);
            Stem* stem = chord2->stem();
            if (stem && chord2->up()) {
                item->p2().ry() = stem->pageBoundingRect().top();
                l2r = vStemDistance;
                item->p2().rx() = stem->pageBoundingRect().right() + stemRight;
            } else {
                item->p2().ry() = chord2->upNote()->pageBoundingRect().top();
                item->p2().rx() = rightNoteEdge + noteRight;
            }
        }
        //
        // special case: one of the bracket endpoints is
        // a rest
        //
        if (!cr1->isChord() && cr2->isChord()) {
            if (item->p2().y() < item->p1().y()) {
                item->p1().setY(item->p2().y());
            } else {
                item->p2().setY(item->p1().y());
            }
        } else if (cr1->isChord() && !cr2->isChord()) {
            if (item->p1().y() < item->p2().y()) {
                item->p2().setY(item->p1().y());
            } else {
                item->p1().setY(item->p2().y());
            }
        }

        // outOfStaff
        if (outOfStaff) {
            double min = cr1->measure()->staffPageBoundingRect(cr1->staffIdx() + move).y();
            if (min < item->p1().y()) {
                item->p1().ry() = min;
                l2l = vStemDistance;
            }
            min = cr2->measure()->staffPageBoundingRect(cr2->staffIdx() + move).y();
            if (min < item->p2().y()) {
                item->p2().ry() = min;
                l2r = vStemDistance;
            }
        }

        // check that slope is no more than max
        double d = (item->p2().y() - item->p1().y()) / (item->p2().x() - item->p1().x());
        if (d < -maxSlope) {
            // move p1 y up
            item->p1().ry() = item->p2().y() + maxSlope * (item->p2().x() - item->p1().x());
        } else if (d > maxSlope) {
            // move p2 y up
            item->p2().ry() = item->p1().ry() + maxSlope * (item->p2().x() - item->p1().x());
        }

        // check for collisions
        size_t n = item->elements().size();
        if (n >= 3) {
            d = (item->p2().y() - item->p1().y()) / (item->p2().x() - item->p1().x());
            for (size_t i = 1; i < (n - 1); ++i) {
                const EngravingItem* e = item->elements().at(i);
                if (e->isChord()) {
                    const Chord* chord = toChord(e);
                    const Stem* stem = chord->stem();
                    if (stem) {
                        RectF r(chord->up() ? stem->pageBoundingRect() : chord->upNote()->pageBoundingRect());
                        double y3 = r.top();
                        double x3 = r.x() + r.width() * .5;
                        double y0 = item->p1().y() + (x3 - item->p1().x()) * d;
                        double c  = y0 - y3;
                        if (c > 0) {
                            item->p1().ry() -= c;
                            item->p2().ry() -= c;
                        }
                    }
                }
            }
        }
    } else {
        if (cr1->isChord()) {
            const Chord* chord1 = toChord(cr1);
            Stem* stem = chord1->stem();
            if (stem) {
                xx1 = stem->pageBoundingRect().x();
            }
            if (!chord1->up() && stem) {
                item->p1().ry() = stem->pageBoundingRect().bottom();
                l2l = vStemDistance;
                item->p1().rx() = stem->pageBoundingRect().left() - stemLeft;
            } else {
                item->p1().ry() = chord1->downNote()->pageBoundingRect().bottom();
                item->p1().rx() = leftNoteEdge - noteLeft;
            }
        }

        if (cr2->isChord()) {
            const Chord* chord2 = toChord(cr2);
            Stem* stem = chord2->stem();
            if (stem && !chord2->up()) {
                item->p2().ry() = stem->pageBoundingRect().bottom();
                l2r = vStemDistance;
                item->p2().rx() = stem->pageBoundingRect().right() + stemRight;
            } else {
                item->p2().ry() = chord2->downNote()->pageBoundingRect().bottom();
                item->p2().rx() = rightNoteEdge + noteRight;
            }
        }
        //
        // special case: one of the bracket endpoints is
        // a rest
        //
        if (!cr1->isChord() && cr2->isChord()) {
            if (item->p2().y() > item->p1().y()) {
                item->p1().setY(item->p2().y());
            } else {
                item->p2().setY(item->p1().y());
            }
        } else if (cr1->isChord() && !cr2->isChord()) {
            if (item->p1().y() > item->p2().y()) {
                item->p2().setY(item->p1().y());
            } else {
                item->p1().setY(item->p2().y());
            }
        }
        // outOfStaff
        if (outOfStaff) {
            double max = cr1->measure()->staffPageBoundingRect(cr1->staffIdx() + move).bottom();
            if (max > item->p1().y()) {
                item->p1().ry() = max;
                l2l = vStemDistance;
            }
            max = cr2->measure()->staffPageBoundingRect(cr2->staffIdx() + move).bottom();
            if (max > item->p2().y()) {
                item->p2().ry() = max;
                l2r = vStemDistance;
            }
        }
        // check that slope is no more than max
        double d = (item->p2().y() - item->p1().y()) / (item->p2().x() - item->p1().x());
        if (d < -maxSlope) {
            // move p1 y up
            item->p2().ry() = item->p1().y() - maxSlope * (item->p2().x() - item->p1().x());
        } else if (d > maxSlope) {
            // move p2 y up
            item->p1().ry() = item->p2().ry() - maxSlope * (item->p2().x() - item->p1().x());
        }

        // check for collisions
        size_t n = item->elements().size();
        if (n >= 3) {
            d  = (item->p2().y() - item->p1().y()) / (item->p2().x() - item->p1().x());
            for (size_t i = 1; i < (n - 1); ++i) {
                EngravingItem* e = item->elements()[i];
                if (e->isChord()) {
                    const Chord* chord = toChord(e);
                    const Stem* stem = chord->stem();
                    if (stem) {
                        RectF r(chord->up() ? chord->downNote()->pageBoundingRect() : stem->pageBoundingRect());
                        double y3 = r.bottom();
                        double x3 = r.x() + r.width() * .5;
                        double y0 = item->p1().y() + (x3 - item->p1().x()) * d;
                        double c  = y0 - y3;
                        if (c < 0) {
                            item->p1().ry() -= c;
                            item->p2().ry() -= c;
                        }
                    }
                }
            }
        }
    }

    if (!cr1->isChord()) {
        item->p1().rx() = cr1->pageBoundingRect().left() - noteLeft;
    }
    if (!cr2->isChord()) {
        Shape shape = cr2->ldata()->shape();
        auto shEl = shape.find_if([](const ShapeElement& i) { return i.item() && i.item()->isChordRest(); });
        DO_ASSERT(shEl);
        item->p2().rx() = shEl->translated(cr2->pagePos()).right() + noteRight;
    }

    PointF mp(item->parentItem()->pagePos());
    if (item->explicitParent()->isMeasure()) {
        System* s = toMeasure(item->explicitParent())->system();
        if (s) {
            SysStaff* tupletStaff = s->staff(item->vStaffIdx());
            mp.ry() += tupletStaff ? tupletStaff->y() : 0.0;
        }
    }
    item->p1() -= mp;
    item->p2() -= mp;

    item->p1() += item->userP1();
    item->p2() += item->userP2();
    xx1 -= mp.x();

    item->p1().ry() -= l2l * (item->isUp() ? 1.0 : -1.0);
    item->p2().ry() -= l2r * (item->isUp() ? 1.0 : -1.0);

    const double yOffset = item->staffOffsetY();

    item->p1().ry() -= yOffset;
    item->p2().ry() -= yOffset;

    if (style.styleB(Sid::tupletExtendToEndOfDuration)) {
        extendToEndOfDuration(item, toChordRest(cr2));
    }

    // l2l l2r, mp, _p1, _p2 const

    // center number
    double xNumber = 0.0;
    double numberWidth = 0.0;
    if (item->number()) {
        Text::LayoutData* numLdata = item->number()->mutldata();
        TLayout::layoutText(item->number(), numLdata);
        numberWidth = numLdata->bbox().width();

        double yNumber = item->p1().y() + (item->p2().y() - item->p1().y()) * .5 - l1 * (item->isUp() ? 1.0 : -1.0);

        if (placeNumberOnRhythmicCenter(item, cr1, cr2, ctx)) {
            xNumber = computeRhythmicCenter(item, cr2);
        } else if (cr1->beam() && cr2->beam() && cr1->beam() == cr2->beam() && !item->hasBracket()) {
            // for beamed tuplets, center number on beam - if they don't have a bracket
            const ChordRest* crr = toChordRest(cr1);
            if (item->isUp() == crr->up()) {
                double deltax = cr2->pagePos().x() - cr1->pagePos().x();
                xNumber = xx1 + deltax * .5;
            } else {
                double deltax = item->p2().x() - item->p1().x();
                xNumber = item->p1().x() + deltax * .5;
            }
        } else {
            // otherwise center on the bracket (TODO: make centering rules customizable?)
            double deltax = item->p2().x() - item->p1().x();
            xNumber = item->p1().x() + deltax * .5;
        }

        numLdata->setPos(PointF(xNumber, yNumber) - item->ldata()->pos());
    }

    if (item->hasBracket()) {
        double slope = (item->p2().y() - item->p1().y()) / (item->p2().x() - item->p1().x());
        const double numberGap = 0.35 * spatium;

        if (item->isUp()) {
            if (item->number()) {
                //set width of bracket hole
                double x     = xNumber - numberWidth * .5 - numberGap;
                item->p1().rx() = std::min(item->p1().x(), x - 0.5 * l1); // ensure enough space for the number
                double y     = item->p1().y() + (x - item->p1().x()) * slope;
                item->bracketL[0] = PointF(item->p1().x(), item->p1().y());
                item->bracketL[1] = PointF(item->p1().x(), item->p1().y() - l1);
                item->bracketL[2] = PointF(x,   y - l1);

                //set width of bracket hole
                x           = xNumber + numberWidth * .5 + numberGap;
                item->p2().rx() = std::max(item->p2().x(), x + 0.5 * l1); // ensure enough space for the number
                y           = item->p1().y() + (x - item->p1().x()) * slope;
                item->bracketR[0] = PointF(x,   y - l1);
                item->bracketR[1] = PointF(item->p2().x(), item->p2().y() - l1);
                item->bracketR[2] = PointF(item->p2().x(), item->p2().y());
            } else {
                item->bracketL[0] = PointF(item->p1().x(), item->p1().y());
                item->bracketL[1] = PointF(item->p1().x(), item->p1().y() - l1);
                item->bracketL[2] = PointF(item->p2().x(), item->p2().y() - l1);
                item->bracketL[3] = PointF(item->p2().x(), item->p2().y());
            }
        } else {
            if (item->number()) {
                //set width of bracket hole
                double x     = xNumber - numberWidth * .5 - numberGap;
                item->p1().rx() = std::min(item->p1().x(), x - 0.5 * l1); // ensure enough space for the number
                double y     = item->p1().y() + (x - item->p1().x()) * slope;
                item->bracketL[0] = PointF(item->p1().x(), item->p1().y());
                item->bracketL[1] = PointF(item->p1().x(), item->p1().y() + l1);
                item->bracketL[2] = PointF(x,   y + l1);

                //set width of bracket hole
                x           = xNumber + numberWidth * .5 + numberGap;
                item->p2().rx() = std::max(item->p2().x(), x + 0.5 * l1);
                y           = item->p1().y() + (x - item->p1().x()) * slope;
                item->bracketR[0] = PointF(x,   y + l1);
                item->bracketR[1] = PointF(item->p2().x(), item->p2().y() + l1);
                item->bracketR[2] = PointF(item->p2().x(), item->p2().y());
            } else {
                item->bracketL[0] = PointF(item->p1().x(), item->p1().y());
                item->bracketL[1] = PointF(item->p1().x(), item->p1().y() + l1);
                item->bracketL[2] = PointF(item->p2().x(), item->p2().y() + l1);
                item->bracketL[3] = PointF(item->p2().x(), item->p2().y());
            }
        }
    }
}

bool TupletLayout::placeNumberOnRhythmicCenter(Tuplet* item, const ChordRest* cr1, const ChordRest* cr2, LayoutContext& ctx)
{
    if (ctx.conf().styleB(Sid::tupletNumberRythmicCenter) && !isSymmetric(item, cr1, cr2)) {
        Fraction center = centerTick(item);
        if (cr2->tick() <= center) {
            return ctx.conf().styleB(Sid::tupletExtendToEndOfDuration);
        }

        return true;
    }

    return false;
}

bool TupletLayout::isSymmetric(Tuplet* item, const ChordRest* cr1, const ChordRest* cr2)
{
    Fraction endTick = cr2->endTick();

    std::vector<Segment*> tupletSegments;
    tupletSegments.reserve(item->elements().size());

    for (Segment* segment = cr1->segment(); segment && segment->tick() < endTick; segment = segment->nextActive()) {
        tupletSegments.push_back(segment);
    }

    size_t segmentsCount = tupletSegments.size();
    for (size_t i = 0; i < segmentsCount; ++i) {
        size_t j = segmentsCount - 1 - i;
        if (j <= i) {
            break;
        }
        Segment* firstSeg = tupletSegments[i];
        Segment* secondSeg = tupletSegments[j];
        if (firstSeg->ticks() != secondSeg->ticks()) {
            return false;
        }
    }

    return true;
}

double TupletLayout::computeRhythmicCenter(Tuplet* item, const ChordRest* endChord)
{
    Fraction center = centerTick(item);

    staff_idx_t track = item->track();
    const Segment* refSeg = nullptr;
    const ChordRest* refCR = nullptr;
    for (Segment* seg = endChord->segment(); seg; seg = seg->prev(SegmentType::ChordRest)) {
        refSeg = seg;
        refCR = toChordRest(seg->element(track));
        if (!refCR) {
            continue;
        }
        if (seg->tick() <= center) {
            break;
        }
    }

    IF_ASSERT_FAILED(refSeg && refCR) {
        return 0.0;
    }

    if (refSeg->tick() == center) {
        double xRef = 0.0;
        if (refCR->isChord()) {
            const Chord* chord = toChord(refCR);
            bool chordUp = chord->up();
            bool tupletPlaceAbove = item->isUp();
            xRef = 0.5 * chord->upNote()->headWidth();
            if (chordUp == tupletPlaceAbove) {
                // Stem-side correction
                xRef += (chordUp ? 0.25 : -0.25) * chord->upNote()->headWidth();
            }
        } else {
            xRef = 0.5 * refCR->width();
        }
        return refSeg->x() + xRef;
    }

    Fraction refCRTicks = refCR->actualTicks();
    Fraction tickDiff = center - refCR->tick();
    double tickRatio = (tickDiff / refCRTicks).toDouble();

    const Segment* nextSeg = item->measure()->findSegment(SegmentType::ChordRest, refSeg->tick() + refCRTicks);
    double xRef = refSeg->x() + (refCR->isChord() ? toChord(refCR)->upNote()->headWidth() : refCR->width());
    double refWidth = nextSeg ? nextSeg->x() - xRef : refSeg->width();

    double relativeWidth = refWidth * tickRatio;

    return xRef + relativeWidth;
}

Fraction TupletLayout::centerTick(Tuplet* item)
{
    Fraction startTick = item->tick();
    Fraction baseLen = item->baseLen().ticks();
    Fraction ratio = item->ratio();
    Fraction subdivision = item->baseLen().ticks() / item->ratio();
    Fraction endTick = startTick + baseLen * ratio.denominator();

    Fraction centerTick = (startTick + endTick - subdivision) / 2;

    return centerTick;
}

void TupletLayout::extendToEndOfDuration(Tuplet* item, const ChordRest* endCR)
{
    Fraction baseDuration = item->baseLen().ticks();
    if (endCR->ticks() <= baseDuration) {
        return;
    }

    Fraction lastTupletSubdivision = endCR->endTick() - baseDuration / item->ratio();
    Segment* refSegment = endCR->segment();
    while (refSegment) {
        Segment* nextCRSeg = refSegment->next1(SegmentType::ChordRest);
        if (!nextCRSeg || nextCRSeg->tick() > lastTupletSubdivision) {
            break;
        }
        refSegment = nextCRSeg;
    }

    Fraction tickRatio = (lastTupletSubdivision - refSegment->tick()) / refSegment->ticks();

    double xResult = refSegment->pagePos().x() + refSegment->width() * tickRatio.toDouble() + item->score()->noteHeadWidth();

    const double padding = 0.6 * item->spatium();

    Segment* nextSeg = refSegment->next1WithElemsOnStaff(endCR->vStaffIdx(), ~SegmentType::TimeTick);
    xResult = std::min(xResult, nextSeg->pagePos().x() - padding);

    track_idx_t startTrack = staff2track(endCR->vStaffIdx());
    track_idx_t endTrack = startTrack + VOICES;
    if (nextSeg->isChordRestType()) {
        for (track_idx_t track = startTrack; track < endTrack; ++track) {
            if (ChordRest* chordRest = toChordRest(nextSeg->element(track))) {
                if (Tuplet* nextTuplet = chordRest->tuplet()) {
                    if (nextTuplet->elements().front() == chordRest && nextTuplet->isUp() == item->isUp()) {
                        double xStartOfNextTuplet = nextTuplet->pagePos().x() + nextTuplet->p1().x();
                        xResult = std::min(xResult, xStartOfNextTuplet - padding);
                    }
                }
            }
        }
    }

    Shape nextSegShape = nextSeg->staffShape(endCR->vStaffIdx());
    nextSegShape.translate(PointF(nextSeg->pagePos().x(), nextSeg->system()->staff(endCR->vStaffIdx())->y()));
    double yAbove = item->p2().y() - (item->isUp() ? item->style().styleMM(Sid::tupletBracketHookHeight) : 0.0);
    double yBelow = item->p2().y() + (item->isUp() ? 0.0 : item->style().styleMM(Sid::tupletBracketHookHeight));
    double left = nextSegShape.leftMostEdgeAtHeight(yAbove, yBelow);
    xResult = std::min(xResult, left - padding);

    double curPos = item->p2().x();
    item->p2().rx() = std::max(curPos, xResult - item->measure()->pagePos().x());
}
