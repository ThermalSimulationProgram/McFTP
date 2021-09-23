#ifndef _FILEOPERATOR_H
#define _FILEOPERATOR_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include "utils/utils.h"


#define FSTREAM_IN       (0xF)
#define FSTREAM_OUT      (0x0)
#define FSTREAM_TRUNC    (0x1)
#define FSTREAM_APP      (0x2)



class FileOperator {
private:


   bool is_input_active;

   bool isOpen;

   bool isSaved;

   std::string filename;

   int mode;

   std::vector <std::string> content;

   std::ifstream iFile;
   std::ofstream oFile;



public:
   FileOperator(const std::string&, int);
   ~FileOperator();

   double readDouble();

   template <typename T>
   std::vector <T> readVector();

   template <typename T>
   std::vector <std::vector <T> > readMatrix();

   void write(const std::string&);

   void write(const std::vector <std::string>&);

   void save();

   void close();

   void changePermission();
   void changePermission(const std::string&);
};

template <typename T>
std::vector <T> FileOperator::readVector()
{
   std::vector <T> ret;

   if ((!is_input_active) && isOpen)
   {
      return(ret);
   }
   for (int i = 0; i < (int)content.size(); ++i)
   {
      appStringToVector(content[i], ret);
   }

   return(ret);
}

template <typename T>
std::vector <std::vector <T> > FileOperator::readMatrix()
{
   std::vector <std::vector <T> > ret;

   if ((!is_input_active) && isOpen)
   {
      return(ret);
   }
   ret.reserve(content.size());
   for (int i = 0; i < (int)content.size(); ++i)
   {
      ret.push_back(stringToVector <T>(content[i]));
   }

   return(ret);
}

// read a csv file and save the first numbers as double
double loadDoubleFromFile(const std::string&  filename);


template <typename T>
std::vector <T> loadVectorFromFile(const std::string& filename)
{
   FileOperator file(filename, (int)FSTREAM_IN);

   return(file.readVector <T>());
}

template <typename T>
std::vector <std::vector <T> > loadMatrixFromFile(const std::string& filename)
{
   FileOperator file(filename, (int)FSTREAM_IN);

   return(file.readMatrix <T>());
}

void saveContentToNewFile(const std::string& filename,
                          const std::vector <std::string>&data);

void appendContentToFile(const std::string& filename,
                         const std::vector <std::string>&data);

template <typename T>
void saveToNewFile(const std::string& filename,
                   const std::vector <T>& data)
{
   saveContentToNewFile(filename,
                        std::vector <std::string>(1, vectorTostring(data)));
}

template <typename T>
void saveToNewFile(const std::string& filename,
                   const std::vector <T>& data, int columnNumberLimit)
{
   saveContentToNewFile(filename,
                        vectorTostringMatrix(data, columnNumberLimit));
}

template <typename T>
void saveToNewFile(const std::string& filename,
                   const std::vector <std::vector <T> >& data)
{
   saveContentToNewFile(filename, matrixTostring(data));
}

template <typename T>
void appendToFile(const std::string& filename,
                  const std::vector <T>& data)
{
   appendContentToFile(filename,
                       std::vector <std::string>(1, vectorTostring(data)));
}

template <typename T>
void appendToFile(const std::string& filename,
                  const std::vector <std::vector <T> >&data)
{
   // std::vector<std::string>  ss = matrixTostring(data);

   // for (int i = 0; i < (int)ss.size(); ++i)
   // {
   //    std::cout << ss[i] << std::endl;
   // }

   appendContentToFile(filename, matrixTostring(data));
}

template <typename T>
void appendToFile(const std::string& filename,
                  const std::vector <T>& data, int columnNumberLimit)
{
   appendContentToFile(filename,
                       vectorTostringMatrix(data, columnNumberLimit));
}

#endif
