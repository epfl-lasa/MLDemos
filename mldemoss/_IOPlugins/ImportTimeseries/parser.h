/*********************************************************************
MLDemos: A User-Friendly visualization toolkit for machine learning
Copyright (C) 2011 Chrstophe Paccolat
Contact: mldemos@b4silio.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License,
version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/

#ifndef PARSER_H
#define PARSER_H

#define UNSIGNED_INT_TYPE   1
#define INT_TYPE            2
#define FLOAT_TYPE          4
#define DOUBLE_TYPE         8
#define CHAR_TYPE           16
#define STRING_TYPE         32
#define UNKNOWN_TYPE        64
#define ALL_TYPES           127
#define NUMERIC_TYPES       (UNSIGNED_INT_TYPE | INT_TYPE | FLOAT_TYPE | DOUBLE_TYPE)

#define MISSING_VALUE        "?"

#include <map>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <types.h>

using namespace std;

template<typename T>
bool is(const std::string &s)
{
    std::istringstream stream(s);
    T instance;
    return ((stream >> instance) && (stream >> std::ws) && stream.eof());
}

class CSVRow
{
public:
    std::string const& operator[](std::size_t index) const;
    std::string at(size_t column) const;
    std::string getFirstCell() const;
    std::string getLastCell() const;
    std::vector<std::string> getParsedLine() const;
    std::size_t size() const;

    void readNextRow(std::istream& str);

private:
    std::vector<std::string>    m_data;
};

class CSVIterator
{
public:
    typedef std::input_iterator_tag     iterator_category;
    typedef CSVRow                      value_type;
    typedef std::size_t                 difference_type;
    typedef CSVRow*                     pointer;
    typedef CSVRow&                     reference;

    CSVIterator(std::istream& str);
    CSVIterator();

    bool eof();

    // Pre Increment
    CSVIterator& operator++();

    // Post increment
    CSVIterator operator++(int);

    CSVRow const& operator*()   const;
    CSVRow const* operator->()  const;
    bool operator!=(CSVIterator const& rhs);
    bool operator==(CSVIterator const& rhs);

private:
    std::istream*       m_str;
    CSVRow              m_row;
};

class CSVParser
{
public:
    void parse(const char* fileName);
    vector<size_t> getMissingValIndex();
    void cleanData(unsigned int acceptedTypes);
    pair<vector<fvec>,ivec> getData(unsigned int acceptedTypes = ALL_TYPES);
	vector<vector<string> >& Data(){return data;};
private:
    ifstream file;
    map<string,unsigned int> classLabels;
    vector<vector<string> > data;
    map<unsigned int, unsigned int> inputTypes;
};

#endif // PARSER_H
