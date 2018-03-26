#include "serializable.h"
#include "serialize.h"
#include <cstdlib>
#include <easylogging++.cc>
#include <easylogging++.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <yaml-cpp/yaml.h>

INITIALIZE_EASYLOGGINGPP

std::string random_string(size_t length)
{
  auto randchar = []() -> char {
    const char charset[]
      = "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

class TestClass_1 : public proplib::Serializable
{
public:
  std::vector<int>           vector_int;
  std::vector<float>         vector_float;
  float                      float_field;
  int                        int_field;
  std::string                string_field;
  std::map<int, std::string> map_int_string;

  bool bool_field;

  TestClass_1() {}
  void set_random()
  {
    for (int i = 0; i < 10; i++)
    {
      vector_int.push_back(rand());
      vector_float.push_back((float)rand() / RAND_MAX);
    }

    float_field = (float)rand() / RAND_MAX;
    int_field = rand();

    string_field = random_string(20);
    map_int_string[rand()] = random_string(5);

    bool_field = rand()%2;
  }

  bool operator==(const TestClass_1& other)
  {
    if (fabs(float_field - other.float_field) > std::numeric_limits<float>::epsilon())
      return false;

    if (int_field != other.int_field)
      return false;

    if (bool_field != other.bool_field)
      return false;

    if (string_field != other.string_field)
      return false;

    if (vector_int != other.vector_int)
      return false;

    if (vector_float.size() != other.vector_float.size())
      return false;

    for (int i = 0; i < vector_float.size(); i++)
    {
      if (fabs(vector_float[i] - other.vector_float[i]) > std::numeric_limits<float>::epsilon())
        return false;
    }

    if (map_int_string != other.map_int_string)
      return false;

    return true;
  }

  void reset()
  {
    vector_int.clear();
    vector_float.clear();
    float_field = 0.f;
    int_field = 0;
    string_field = "";
    map_int_string.clear();
    bool_field = false;
  }

private:
  SERIALIZE(vector_int);
  SERIALIZE(vector_float);
  SERIALIZE(float_field);
  SERIALIZE(int_field);
  SERIALIZE(string_field);
  SERIALIZE(map_int_string);
  SERIALIZE(bool_field);
};

class TestClass_2 : public proplib::Serializable
{

public:
  std::vector<TestClass_1*> vector_test_class_1;
  TestClass_1               test_class_1_field;

  std::map<std::string, Serializable*> childs;

  bool bool_field;

  TestClass_2() {}

  void reset()
  {
    vector_test_class_1.clear();
    test_class_1_field.reset();
    bool_field = false;
  }

  void set_random()
  {
    for (int i = 0; i < 10; i++)
    {
      vector_test_class_1.push_back(new TestClass_1());
      vector_test_class_1.back()->set_random();

      test_class_1_field.set_random();
    }

    bool_field = rand()%2;
  }

  bool operator==(const TestClass_2& other)
  {
    if (!(test_class_1_field == other.test_class_1_field))
      return false;

    if (!(bool_field == other.bool_field))
      return false;

    if (vector_test_class_1.size() != other.vector_test_class_1.size())
      return false;

    for (int i = 0; i < vector_test_class_1.size(); i++)
    {
      if (!(*vector_test_class_1[i] == *other.vector_test_class_1[i]))
        return false;
    }
    return true;
  }

private:
  SERIALIZE(vector_test_class_1);
  SERIALIZE(test_class_1_field);
  SERIALIZE(bool_field);
};

void main()
{
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

  TestClass_2 ser_test;

  ser_test.set_random();

  YAML::Emitter out;
  out << YAML::BeginMap;
  std::string name = "serialize";

  el::Logger* businessLogger = el::Loggers::getLogger(name);

  el::Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level %msg");

  el::Loggers::reconfigureLogger(name, defaultConf);

  std::string name2 = name;

  ser_test.set_logger(name);
  ser_test.serialize(out);

  out << YAML::EndMap;
  std::cout << "yaml\n" << out.c_str() << std::endl;
  std::ofstream file;

  file.open("test.yml");
  file << out.c_str() << std::endl;
  file.close();

  YAML::Node saved_yml = YAML::LoadFile("test.yml");

  TestClass_2 deser_test;

  deser_test.reset();

  std::cout << "-----------------------" << std::endl;
  std::cout << saved_yml << std::endl;

  deser_test.set_logger(name2);
  proplib::res_t res = deser_test.deserialize(saved_yml);

  bool test_passed = ser_test == deser_test;

  std::cout << "Test passed: " << test_passed << std::endl;

  int d;
  d = 0;
}
