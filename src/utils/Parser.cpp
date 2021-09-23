#include "utils/Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

#include <stdexcept>

#include "core/Dispatcher.h"
#include "dispatchers/Aperiodic.h"
#include "dispatchers/Periodic.h"
#include "dispatchers/PeriodicJitter.h"

#include "configuration/Scratch.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"
#include "utils/FileOperator.h"
#include "core/structdef.h"
#include "utils/TemperatureSaveOption.h"
#include "utils/SoftTemperatureSaveOption.h"
#include "benchmarks/benchmarks.h"
#include "../McFTPBuildConfig.h"


#include "performance_counter/PerformanceCounters.h"



using namespace pugi;
using namespace std;

void stringToLowerCase(string& s)
{
   transform(s.begin(), s.end(), s.begin(), ::tolower);
}

bool string2bool(const string& s)
{
   if (s == "false")
   {
      return(false);
   }
   else if (s == "true")
   {
      return(true);
   }
   else
   {
      throw invalid_argument("invalid input string to function string2bool");
   }
}

void checkNodeExists(const xml_node& n, const string& parentName,
                     const string& nodeName)
{
   if (n == NULL)
   {
      throw XmlNodeNotFound(nodeName, parentName);
   }
}

xml_node getNode(const xml_node& p, const string& parentName,
                 const string& nodeName)
{
   xml_node n = p.child(nodeName.c_str());

   checkNodeExists(n, parentName, nodeName);
   return(n);
}

xml_node getNode(const xml_document& p, const string& parentName,
                 const string& nodeName)
{
   xml_node n = p.child(nodeName.c_str());

   checkNodeExists(n, parentName, nodeName);
   return(n);
}

string getAttributeAsString(const xml_node& n, const string& nodeName,
                            const string& attrName)
{
   string value = n.attribute(attrName.c_str()).value();

   if (value.length() == 0)
   {
      throw XmlAttributeEmpty(nodeName + "'s " + attrName);
   }
   return(value);
}

string getChildAttributeAsString(const xml_node& n,
                                 const string& parentName, const string& nodeName, const string attrName)
{
   xml_node c = getNode(n, parentName, nodeName);

   return(getAttributeAsString(c, nodeName, attrName));
}

int getAttributeAsInt(const xml_node& n, const string& nodeName,
                      const string& attrName)
{
   checkNodeExists(n, "given xml file", nodeName);
   string value = getAttributeAsString(n, nodeName, attrName);
   int    ret   = 0;

   try{
      ret = (int)stoul(value, NULL, 0);
   }catch (...) {
      throw XmlAttributeInvalid(nodeName + "'s " + attrName);
   }
   return(ret);
}

int getAttributeAsPositiveInt(const xml_node& n, const string& nodeName,
                              const string& attrName)
{
   int ret = getAttributeAsInt(n, nodeName, attrName);

   if (0 > ret)
   {
      throw XmlAttributeInvalid(nodeName + "'s " + attrName);
   }
   return(ret);
}

double getAttributeAsDouble(const xml_node& n, const string& nodeName,
                            const string& attrName)
{
   checkNodeExists(n, "given xml file", nodeName);
   string value = getAttributeAsString(n, nodeName, attrName);
   double ret   = 0.0;

   try{
      ret = (double)strtod(value.c_str(), 0);
   }catch (...) {
      throw XmlAttributeInvalid(nodeName + "'s " + attrName);
   }
   return(ret);
}

bool getAttributeAsBool(const xml_node& n, const string& nodeName,
                        const string& attrName, bool def)
{
   bool ret = def;

   if (n)
   {
      string value = getAttributeAsString(n, nodeName, attrName);
      try{
         ret = string2bool(value);
      }catch (...) {
         throw XmlAttributeInvalid(nodeName + "'s " + attrName);
      }
   }
   return(ret);
}

bool getAttributeAsBool(const xml_node& n, const string& nodeName,
                        const string& attrName)
{
   return(getAttributeAsBool(n, nodeName, attrName, false));
}

void parseTemperatureSaveOptions(xml_node n, string nodeName,
                                 TemperatureSaveOption& op)
{
   op.isSaveGlobalPeakTemperature = getAttributeAsBool(
      n.child("global_peak_temperature"),
      nodeName + "'s global_peak_temperature", "save", true);

   op.isSaveSensorPeakTemperature = getAttributeAsBool(
      n.child("sensor_peak_temperature"),
      nodeName + "'s sensor_peak_temperature", "save", true);

   op.isSaveSensorMeanTemperature = getAttributeAsBool(
      n.child("sensor_mean_temperature"),
      nodeName + "'s sensor_mean_temperature", "save", true);

   op.isSaveSensorReadOverhead = getAttributeAsBool(
      n.child("sensor_read_overhead"),
      nodeName + "'s sensor_read_overhead", "save", true);

   op.isSaveSensorTemperatureTrace = getAttributeAsBool(
      n.child("sensor_temperature_trace"),
      nodeName + "'s sensor_temperature_trace", "save", true);
}

void ParserPAPITest(int argc, char **argv)
{
   // PerformanceCounters::PAPI_test(argc, argv);
}

Parser::Parser(string _xml_path)
{
   xml_path = _xml_path;
}

// This function parse the file pointed by xml_path, and then
// save all necessary data required by the simulation in Scratch class.
int Parser::parseFile()
{
   int ret = 0;

   // load the xml file
   xml_document     doc;
   xml_parse_result retval = doc.load_file(xml_path.data());

   if (retval.status != status_ok)
   {
      throw XmlFileLoadException(xml_path, retval.status);
      return(-1);
   }

   // get experiment experimentName and duration
   xml_node experimentNode = getNode(doc, "given xml file", "experiment");
   string   experimentName = getAttributeAsString(experimentNode, "experiment",
                                                  "name");
   // get the experiment duration in microsecond unit
   xml_node      durationNode = getNode(experimentNode, "experiment", "duration");
   unsigned long duration     = parseTimeUsec(durationNode, "duration");
   // load configuration for the experiment proccessor
   xml_node processorNode = getNode(experimentNode, "experiment", "processor");
   // get the number of cores to be used
   int ncores = getAttributeAsPositiveInt(processorNode, "processor",
                                          "core_number");
   // whether to enable frequency scaling and sleeping
   bool fixedFrequency = getAttributeAsBool(processorNode, "processor",
                                            "fixed_frequency");
   bool fixedActive = getAttributeAsBool(processorNode, "processor",
                                         "fixed_active");

   bool          useHardwareTempSensor   = false;
   bool          useSoftwareTempSensor   = false;
   unsigned long temperatureSamplePeriod = 200000;      // default period 200ms

   xml_node temperatureSensorsNode = experimentNode.child("temperature_sensors");
   xml_node hardwareSensorNode;
   xml_node softwareSensorNode;

   if (temperatureSensorsNode)
   {
      xml_node samplePeriodNode = temperatureSensorsNode.child("sample_period");
      if (samplePeriodNode)
      {
         temperatureSamplePeriod = parseTimeUsec(samplePeriodNode,
                                                 "sample_period");
      }

      hardwareSensorNode = temperatureSensorsNode.child("hardware");
                #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
      useHardwareTempSensor = getAttributeAsBool(hardwareSensorNode,
                                                 "hardware Node", "enabled");
                #endif

                #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
      softwareSensorNode    = temperatureSensorsNode.child("software");
      useSoftwareTempSensor = getAttributeAsBool(softwareSensorNode,
                                                 "software Node", "enabled");
                #endif
   }

   // put all the necessary input parameters in scratch
   Scratch::initialize(ncores, duration, experimentName, useHardwareTempSensor,
                       useSoftwareTempSensor);

   Scratch::setFixedFrequency(fixedFrequency);
   Scratch::setFixedActive(fixedActive);
   Scratch::temperatureSamplePeriod = temperatureSamplePeriod;

   // get parameters of all tasks
   xml_node tasksNode = getNode(experimentNode, "experiment", "tasks");
   //iterate through all of the children nodes
   int taskid = 0;
   for (xml_node task = tasksNode.first_child(); task; task = task.next_sibling())
   {
      TaskArgument data = parseTask(task, taskid);
      ++taskid;
      Scratch::addTask(data._type, data._periodicity, data);
   }

   xml_node thermalApproach = getNode(experimentNode, "experiment",
                                      "thermal_approach");
   xml_node kernel = getNode(thermalApproach, "thermal_approach",
                             "kernel");
   string kernelType = getAttributeAsString(kernel,
                                            "thermal_approach kernel", "type");

   if (kernelType == "ptm" || kernelType == "PTM")
   {
      Scratch::setStaticApproach(true);
      xml_node          tonNode  = getNode(kernel, "kernel", "ton");
      xml_node          toffNode = getNode(kernel, "kernel", "toff");
      vector <long int> tonsUS   = parseTimeVectorUsec <long int>(tonNode, "ton");
      vector <long int> toffsUS  = parseTimeVectorUsec <long int>(toffNode, "toff");
      Scratch::setPBOOTons(TimeUtil::Micros(tonsUS));
      Scratch::setPBOOToffs(TimeUtil::Micros(toffsUS));
   }


   // if save the results into files
   TemperatureSaveOption     hardwareOption = TemperatureSaveOption();
   SoftTemperatureSaveOption softwareOption = SoftTemperatureSaveOption();
   xml_node saveFileOptions = experimentNode.child("results");
   if (saveFileOptions)
   {
                #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
      xml_node hardwareSaveNode = saveFileOptions.child("temperature_from_hardware_sensors");
      parseTemperatureSaveOptions(hardwareSaveNode, "temperature_from_hardware_sensors",
                                  Scratch::hardwareOption);
                #endif

                #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
      xml_node softwareSaveNode = saveFileOptions.child("temperature_from_software_sensors");
      parseTemperatureSaveOptions(softwareSaveNode, "temperature_from_software_sensors",
                                  Scratch::softwareOption);
      Scratch::softwareOption.isSavePerformanceCounterValues = getAttributeAsBool(
         softwareSaveNode.child("performance_counter_values"),
         "temperature_from_software_sensors's performance_counter_values",
         "save", true);
                #endif
   }
        #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
   if (useHardwareTempSensor)
   {
      for (xml_node path = hardwareSensorNode.first_child(); path;
           path = path.next_sibling())
      {
         Scratch::hardwareSensorPath.push_back(getAttributeAsString(path,
                                                                    "hardware node's sensor file", "path"));
      }
   }
        #endif

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (useSoftwareTempSensor)
   {
      string softwareNodeName = "temperature_sensors' software";

      // Scratch::softSensorCalculator = getAttributeAsString(softwareSensorNode,
      //    softwareNodeName, "calculator");
      //if (Scratch::softSensorCalculator == "default"){
      xml_node thermalModelParameterNode = getNode(softwareSensorNode,
                                                   softwareNodeName, "thermal_model_parameters");
      string fileW = getChildAttributeAsString(thermalModelParameterNode,
                                               softwareNodeName, "parameterW", "source");
      string fileV = getChildAttributeAsString(thermalModelParameterNode,
                                               softwareNodeName, "parameterV", "source");
      string fileK = getChildAttributeAsString(thermalModelParameterNode,
                                               softwareNodeName, "parameterKtimesTamb", "source");
      string fileInitT = getChildAttributeAsString(thermalModelParameterNode,
                                                   softwareNodeName, "initialT", "source");

      Scratch::softSensorW = loadMatrixFromFile <double>(fileW);
      Scratch::softSensorV = loadMatrixFromFile <double>(fileV);
      Scratch::softSensorK = loadMatrixFromFile <double>(fileK);
      Scratch::ambientT    = loadVectorFromFile <double>(fileInitT);

      xml_node powerModelParameterNode = getNode(softwareSensorNode,
                                                 softwareNodeName, "power_model_parameters");
      string fileA = getChildAttributeAsString(powerModelParameterNode,
                                               softwareNodeName, "coefA", "source");
      string fileB = getChildAttributeAsString(powerModelParameterNode,
                                               softwareNodeName, "coefB", "source");

      Scratch::coefA = loadVectorFromFile <double>(fileA);
      Scratch::coefB = loadVectorFromFile <double>(fileB);
      //}

      xml_node performanceCounterNode = getNode(softwareSensorNode,
                                                softwareNodeName, "performance_counters");

      for (xml_node counter = performanceCounterNode.first_child();
           counter; counter = counter.next_sibling())
      {
         string temp = "performance_counters' " + string(counter.name());
         string name = getAttributeAsString(counter, temp, "name");

         double valueScale = getAttributeAsDouble(counter, temp, "value_scale");
         double weight     = getAttributeAsDouble(counter, temp, "weight");
         Scratch::addSoftTempSensor(name, valueScale, weight);
      }
   }
        #endif

   //Scratch::print();
   //exit(1);
   return(ret);
}

TaskArgument Parser::parseTask(pugi::xml_node task, int taskid)
{
   TaskArgument data = TaskArgument();

   string task_name = getAttributeAsString(task, "task", "name");

   data.taskId = taskid;

   string task_type   = getAttributeAsString(task, "task", "type");
   string load_type   = getAttributeAsString(task, "task", "load_type");
   string periodicity = getAttributeAsString(task, "task", "periodicity");

   _task_type        type;
   _task_periodicity pcity;
   _task_load_type   loadtype;

   data.name = task_name;

   if (task_type == "singlecore")
   {
      type = singlecore;
   }
   else if (task_type == "pipelined")
   {
      type = pipelined;
   }
   else
   {
      throw XmlAttributeInvalid("task " + task_name + "'s type");
   }
   data._type = type;


   xml_node attachedCores = task.child("attached_core");
   int      attached_core;

   if (attachedCores)
   {
      attached_core       = getAttributeAsInt(attachedCores, "attached_core", "value");
      data.default_coreId = attached_core;
   }


   if (load_type == "busy_wait")
   {
      loadtype = busywait;
   }
   else if (load_type == "benchmark")
   {
      loadtype = benchmark;
      xml_node benchmark_node = getNode(task, task_name, "benchmark");
      string   benchmark_name = getAttributeAsString(benchmark_node, "benchmark", "name");
      int      benchmark_id   = getBenchmarkId(benchmark_name);
      if (benchmark_id < 0)
      {
         throw XmlAttributeInvalid(task_name + "'s child benchmark's name");
      }
      data.benchmark_name = benchmark_name;
      data.benchmark_id   = benchmark_id;
   }
   else if (load_type == "user_defined")
   {
      loadtype = userdefined;
      xml_node defined_load_node = getNode(task, task_name, "defined_load");
      int      loadId            = getAttributeAsInt(defined_load_node, "defined_load", "index");
      data.user_defined_load_id = loadId;
   }
   else
   {
      throw XmlAttributeInvalid("task " + task_name + "'s load type");
   }
   data._load_type = loadtype;


   if (periodicity == "periodic")
   {
      pcity       = periodic;
      data.period = parseTime(task.child("period"), task_name + "'s period");
   }
   else if (periodicity == "periodic_jitter")
   {
      pcity       = periodic_jitter;
      data.period = parseTime(task.child("period"), task_name + "'s period");
      data.jitter = parseTime(task.child("jitter"), task_name + "'s jitter");
   }
   else if (periodicity == "aperiodic")
   {
      pcity             = aperiodic;
      data.release_time = parseTime(task.child("release_time"), task_name + "'s release_time");
   }
   else
   {
      throw XmlAttributeInvalid(task_name + "'s periodicity");
   }
   data._periodicity = pcity;


   vector <long int> wcets = parseTimeVectorUsec <long int>(task.child("wcets"), "task " + task_name + "'s child node wcets");

   data.wcets    = TimeUtil::Micros(wcets);
   data.wcets_us = TimeUtil::convert_us(data.wcets);


   return(data);
}

unsigned long Parser::parseTimeUsec(xml_node n, string nodeName)
{
   struct timespec tmp = parseTime(n, nodeName);

   return(TimeUtil::convert_us(tmp));
}

struct timespec Parser::parseTime(xml_node n, string nodeName)
{
   int time_value = getAttributeAsPositiveInt(n, nodeName, "value");

   string units = getAttributeAsString(n, nodeName, "unit");

   struct timespec ret;

   if (units == "sec")
   {
      ret = TimeUtil::Seconds(time_value);
   }
   else if (units == "ms")
   {
      ret = TimeUtil::Millis(time_value);
   }
   else if (units == "us")
   {
      ret = TimeUtil::Micros(time_value);
   }
   else
   {
      throw XmlAttributeInvalid(nodeName + "'s unit");
   }

   return(ret);
}
