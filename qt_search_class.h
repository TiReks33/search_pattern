#ifndef SEARCH_H
#define SEARCH_H

#include <QMainWindow>
//#include "mainwindow.h"

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <string> //for std::getline
#include <sstream> //std::stringstream
#include <vector>

class Search
{

    //Q_OBJECT

//public:
private:

size_t res_size; // size of result array
size_t *p_rs;    // pointer to size
size_t *res_mas; // input pattern result array
//char *pat;
//std::string pat; // pattern string

public:

Search():res_size(0)
    ,p_rs(&res_size)
        //,res_mas(new size_t[*p_rs])
	//,pat(new char[254])
{
    res_mas = new size_t[*result_arr_size()];
}

~Search(){
delete[] res_mas;
//delete[] pat;
}

size_t *& result_arr_size(){return p_rs;}


size_t *& result_array(){return res_mas;}

//std::string & pattern(){return pat;}





static bool is_empty(std::ifstream& pFile);

int clear_memory();

size_t strlenpp(char *str);

size_t strlenpp(const char *str);

size_t strlenpp(const std::string *str);

void show_mas(size_t);

void show_mas_qt_test(std::string const&,char const*);

void show_res(const char*text,const char*pattern);

QString show_finale(const std::string& text,const char*pattern);

std::vector<QString> show_finale_qt(const std::string& text,const char*pattern);

QString show_finale_qt_test(const std::string& text,const char*pattern);

size_t* increase(size_t* data, size_t* m_size, int pointer);

int strstr_multi(const char *text, const char *pattern);

int strstr_multi_qt(const char *text, const char *pattern);

//public slots:

//signals:

void signal(QString string);

};




#endif // SEARCH_H
