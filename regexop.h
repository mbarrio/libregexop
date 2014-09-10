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

#ifndef REGEXOP_H
#define REGEXOP_H

#include <apt_framework/orchestrate.h>
#include <QMultiHash>
#include <QHash>
#include <QString>

#include "regexop_global.h"
#include "substitution.h"

class LIBREGEXOPSHARED_EXPORT RegexOp : public APT_CombinableOperator {
    APT_DECLARE_PERSISTENT(RegexOp);
    APT_DECLARE_RTTI(RegexOp);

public:
    RegexOp();

protected:
    virtual APT_Status initializeFromArgs_(const APT_PropertyList &args, APT_Operator::InitializeContext context);
    virtual APT_Status describeOperator();
    virtual APT_Status doInitialProcessing();
    virtual void processInputRecord(int inputDS);
    virtual APT_Status writeOutputRecord();
    virtual APT_Status doFinalProcessing();

private:
    QMultiHash<QString, Substitution> colSubsts_;
    QList<QString> columns_;
    int numCols_;
    APT_OutputCursor *outCur_;
    QHash<QString, APT_InputAccessorToUString *> inCols_;
    QHash<QString, APT_OutputAccessorToUString *> outCols_;

    QString unquote(const QString &string);
};

#endif // REGEXOP_H
