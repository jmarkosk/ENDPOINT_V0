#ifndef MYDATA_H
#define MYDATA_H

struct sHiveData
{
 //data
 String HiveName; //corresponds to color of cable to the hive
 String TempName; //corresponds to color of cable to the hive
 float HiveTemp;
 String WeightName;
 float HiveWeight;

}HiveData[] = {
  {"SITE_1", "Temp", 1.0, "Weight", 2.0},
  {"SITE_2", "Temp", 3.0, "Weight", 4.0},
  {"SITE_3", "Temp", 5.0, "Weight", 6.0},
  {"SITE_4", "Temp", 7.0, "Weight", 8.0},
};






#endif