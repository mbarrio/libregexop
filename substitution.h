//    libregexop
//    Copyright (C) 2013, 2014  Miguel Barrio Orsikowsky
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published
//    by the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SUBSTITUTION_H
#define SUBSTITUTION_H

#include <QRegExp>
#include <QString>
#include <QDataStream>

class Substitution
{
public:
    Substitution();
    Substitution(const Substitution &other);
    Substitution(const QRegExp &rx, const QString &replace);

    const QRegExp &rx() const { return rx_; }
    const QString &replace() const { return replace_; }

    friend QDataStream &operator <<(QDataStream &stream, const Substitution &subst);
    friend QDataStream &operator >>(QDataStream &stream, Substitution &subst);

private:
    QRegExp rx_;
    QString replace_;
};

#endif // SUBSTITUTION_H
