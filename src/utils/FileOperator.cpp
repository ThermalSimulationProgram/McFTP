#include "utils/FileOperator.h"


using namespace std;

FileOperator::FileOperator(const string& _filename,
                           int _mode) : filename(_filename), mode(_mode),
   iFile(), oFile()
{
   // default values
   is_input_active = false;
   isOpen          = false;
   isSaved         = true;
   content.clear();

   // construct ifsteam and ofstream object
   if (mode == FSTREAM_IN)
   {
      iFile.open(filename, std::ofstream::in);
      is_input_active = true;
   }
   else if (mode == FSTREAM_OUT)
   {
      oFile.open(filename, std::ofstream::out);
   }
   else if (mode == (FSTREAM_OUT | FSTREAM_TRUNC))
   {
      oFile.open(filename, std::ofstream::out | std::ofstream::trunc);
   }
   else if (mode == (FSTREAM_OUT | FSTREAM_APP))
   {
      oFile.open(filename, std::ofstream::out | std::ofstream::app);
   }
   else
   {
      cerr << "FileOperator::FileOperator: invalid mode! Failed to open file: \n"
           << filename << "\n with mode: " << mode << endl;
      exit(1);
   }

   //check if the file is open
   if (is_input_active)
   {
      isOpen = iFile.is_open();
   }
   else
   {
      isOpen = oFile.is_open();
   }

   if (!isOpen)
   {
      cerr << "FileOperator::FileOperator: failed to open given file named: '" <<
         filename << "'" << endl;
      exit(1);
   }

   //read content
   if (is_input_active)
   {
      std::string line;
      while (std::getline(iFile, line))
      {
         content.push_back(line);
      }
   }
}

FileOperator::~FileOperator()
{
   if (!isSaved)
   {
      save();
   }
   close();
   changePermission();
}

double FileOperator::readDouble()
{
   vector <double> temp = readVector <double>();

   if (temp.size() > 0)
   {
      return(temp[0]);
   }
   else
   {
      cerr << "FileOperator::readDouble: failed to read the file!" << endl;
      exit(1);
   }
}

void FileOperator::write(const string& newline)
{
   if ((!is_input_active) && isOpen)
   {
      content.push_back(newline);
      isSaved = false;
   }
}

void FileOperator::write(const vector <string>& lines)
{
   for (int i = 0; i < (int)lines.size(); ++i)
   {
      write(lines[i]);
   }
}

void FileOperator::save()
{
   if ((!is_input_active) && isOpen)
   {
      for (int i = 0; i < (int)content.size(); ++i)
      {
         oFile << content[i] << endl;
      }

      isSaved = true;
      content.clear();
   }
}

void FileOperator::close()
{
   // cout << "FileOperator::close(): closing" << endl;
   if (!isOpen)
   {
      cout << "FileOperator::close(): No file to close!" << endl;
      return;
   }
   if (is_input_active)
   {
      iFile.close();
      isOpen = iFile.is_open();
   }
   else
   {
      oFile.close();
      isOpen = oFile.is_open();
   }

   content.clear();

   if (isOpen)
   {
      cerr << "FileOperator::close(): failed to close given file named: '"
           << filename << "'" << endl;
      exit(1);
   }
}

void FileOperator::changePermission()
{
   changePermission("666");
}

void FileOperator::changePermission(const string& permission)
{
   if (system(("chmod " + permission + " " + filename).data()))
   {
      cout << "FileOperator::changePermission: Failed to change permission of file "
           << filename << endl;
      exit(1);
   }
}

double loadDoubleFromFile(const string& filename)
{
   FileOperator file(filename, FSTREAM_IN);

   return(file.readDouble());
}

void saveContentToNewFile(const string& filename, const vector <string>& data)
{
   FileOperator file(filename, (int)(FSTREAM_OUT | FSTREAM_TRUNC));

   file.write(data);
}

void appendContentToFile(const string& filename, const vector <string>& data)
{
   FileOperator file(filename, (int)(FSTREAM_OUT | FSTREAM_APP));

   file.write(data);
}
