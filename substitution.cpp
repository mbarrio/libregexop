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

#include "substitution.h"

Substitution::Substitution()
{
}

Substitution::Substitution(const Substitution &other)
{
    rx_ = other.rx_;
    replace_ = other.replace_;
}

Substitution::Substitution(const QRegExp &rx, const QString &replace)
{
    rx_ = rx;
    replace_ = replace;
}

QDataStream &operator <<(QDataStream &stream, const Substitution &subst)
{
    return stream << subst.rx_ << subst.replace_;
}

QDataStream &operator >>(QDataStream &stream, Substitution &subst)
{
    return stream >> subst.rx_ >> subst.replace_;
}
