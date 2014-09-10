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

#include <apt_framework/orchestrate.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QString>
#include <QList>

#include "regexop.h"

#define ARGS_DESC \
"{ "\
    "column = { "\
        "description = 'Input column name', "\
        "value = { "\
            "type = { fieldName, input }, "\
            "usageName = 'name' "\
        "}, "\
        "subArgDesc = { "\
            "pattern = { "\
                "description = 'Matching pattern', "\
                "value = { "\
                    "type = { ustring }, "\
                    "usageName = 'pattern' "\
                "} "\
            "}, "\
            "replacement = { "\
                "description = 'Replacement expression', "\
                "value = { "\
                    "type = { ustring }, "\
                    "usageName = 'replacement' "\
                "} "\
            "} "\
        "} "\
    "} "\
"}"
#define MESSAGE_ID_BASE 0
APT_DEFINE_OSH_NAME(RegexOp, regexOp, ARGS_DESC);
APT_IMPLEMENT_RTTI_ONEBASE(RegexOp, APT_CombinableOperator);
APT_IMPLEMENT_PERSISTENT(RegexOp);

RegexOp::RegexOp()
{
}

QString RegexOp::unquote(const QString &string)
{
    if (string.startsWith('\'') && string.endsWith('\'')) {
        return string.mid(1, string.length() - 2);
    } else {
        if (string.startsWith('"') && string.endsWith('"')) {
            return string.mid(1, string.length() - 2);
        } else {
            return string;
        }
    }
}

APT_Status RegexOp::initializeFromArgs_(const APT_PropertyList &args, APT_Operator::InitializeContext context)
{
    APT_Status status = APT_StatusOk;

    if (context == APT_Operator::eRun) return status;

    // Read properties
    for (int i = 0; i < args.count(); i++) {
        const APT_Property & prop = args[i];

        // Name of the column to process
        if (prop.name() == "column") {
            APT_UString colName = prop
                                  .valueList()
                                  .getProperty("value")
                                  .valueAsUString();
            const APT_PropertyList &subArgList = prop.valueList()
                                                 .getProperty("subArgs")
                                                 .valueList();

            APT_UString pattern, replacement;
            for (int j = 0; j < subArgList.count(); j++) {
                const APT_Property &subArg = subArgList[j];

                // Pattern to match
                if (subArg.name() == "pattern") {
                    pattern = subArg
                              .valueList()
                              .getProperty("value")
                              .valueAsUString();
                }

                // Replacement string
                if (subArg.name() == "replacement") {
                    replacement = subArg
                                  .valueList()
                                  .getProperty("value")
                                  .valueAsUString();
                }
            }

            // Store column name -> substitution
            colSubsts_.insert(QString::fromUtf16(colName.content()),
                              Substitution(QRegExp(unquote(QString::fromUtf16(pattern.content()))),
                                           unquote(QString::fromUtf16(replacement.content()))));
        }
    }

    return status;
}

void RegexOp::serialize(APT_Archive &archive, APT_UInt8)
{
    QByteArray a;
    QBuffer buffer(&a);
    QDataStream str(&buffer);

    /* QT objects do not implement the operator||() method,
       so we must use operator<<() and operator>>() logic against
       QDataStream stored in a QByteArray */

    // Store
    if (archive.isStoring()) {
        buffer.open(QIODevice::WriteOnly);
        str << colSubsts_;   // dump data into stream
        buffer.close();
        archive << a.size(); // insert size of data
        for (int i = 0; i < a.size(); ++i) {
            APT_UInt8 byte;
            byte = a[i];
            archive << byte; // insert data, one byte at a time
        }
    }

    // Load
    else if (archive.isLoading()) {
        buffer.open(QIODevice::ReadOnly);
        int size;
        archive >> size;     // read size of data
        for (int i = 0; i < size && !archive.isEof(); ++i) {
            APT_UInt8 byte;
            archive >> byte; // read data, one byte at a time
            a[i] = byte;
        }
        str >> colSubsts_;   // load data from stream
        buffer.close();
    }
}

APT_Status RegexOp::describeOperator()
{
    APT_ErrorLog &eLog = errorLog();

    // Parallel operator
    setKind(APT_Operator::eParallel);

    // Only one input dataset and one output dataset
    setInputDataSets(1);
    setOutputDataSets(1);

    // Get list of columns to process
    QList<QString> columns = colSubsts_.uniqueKeys();
    int numCols = columns.count();

    // Set input and output schema
    APT_Schema tempSchema = viewAdaptedSchema(0);
    APT_Schema inSchema, outSchema;
    for (int i = 0; i < numCols; ++i) {
        APT_FieldSelector inFS = APT_UString(columns[i].utf16());
        APT_SchemaField field = tempSchema.field(inFS);
        if (field.typeSpec().type() != APT_SchemaTypeSpec::eUString) {
            *eLog << "Type of column \"" << field.Uidentifier() << "\" must be ustring." << endl;
            eLog.logError(MESSAGE_ID_BASE + 1);
            return APT_StatusFailed;
        }
        inSchema.addField(field);
        outSchema.addField(field);
    }
    APT_SchemaField inField, outField;
    inField.setIdentifier("inRec");
    outField.setIdentifier("outRec");
    inField.setTypeSpec("*");
    outField.setTypeSpec("*");
    inSchema.addField(inField);
    outSchema.addField(outField);
    setInputInterfaceSchema(inSchema, 0);
    setOutputInterfaceSchema(outSchema, 0);

    // Declare transfer from input records to output records
    declareTransfer("inRec", "outRec", 0, 0);

    return APT_StatusOk;
}

APT_Status RegexOp::doInitialProcessing()
{
    // List of columns to be processed
    columns_ = colSubsts_.uniqueKeys();
    numCols_ = columns_.count();

    // Setup input and output accessors for each column
    APT_InputAccessorInterface *inAcc = inputAccessorInterface(0);
    outCur_ = outputCursor(0);
    for (int i = 0; i < numCols_; ++i) {
        QString colName = columns_.value(i);
        inCols_[colName] = new APT_InputAccessorToUString();
        inAcc->setupAccessor(colName.toStdString().data(), inCols_[colName]);
        outCols_[colName] = new APT_OutputAccessorToUString(colName.toStdString().data(), outCur_);
    }

    // Process inputs in no particular order
    setInputConsumptionPattern(eAnyInput);

    return APT_StatusOk;
}

void RegexOp::processInputRecord(int inputDS)
{
    if (inputDS == 0) {
        for (int i = 0; i < numCols_; ++i) {
            QString colName = columns_[i];
            APT_InputAccessorToUString *inAccessor = inCols_.value(colName);

            /* Do all the defined substitutions for the current column.
               The substitutions' list is stored in reverse order, so we
               have to process it backwards */

            APT_UInt32 vectorLength = inAccessor->vectorLength();
            for (APT_UInt32 j = 0; j < vectorLength; j++) {
                if (! inAccessor->isNullAt(j)) {
                    QString colValue = QString::fromUtf16(inAccessor->valueAt(j).content(),
                                                          inAccessor->valueAt(j).length());
                    QList<Substitution> substs = colSubsts_.values(colName);
                    for (int k = substs.size() - 1; k >= 0; --k) {
                        Substitution subst = substs.value(k);
                        colValue.replace(subst.rx(), subst.replace());
                    }

                    APT_UString outValue = APT_UString(colValue.utf16());
                    outCols_.value(colName)->setValueAt(j, outValue);
                }
            }
        }

        transferAndPutRecord(0);
    }
}

APT_Status RegexOp::writeOutputRecord()
{
    outCur_->putRecord();

    return APT_StatusOk;
}

APT_Status RegexOp::doFinalProcessing()
{
    outCur_->done();

    return APT_StatusOk;
}
