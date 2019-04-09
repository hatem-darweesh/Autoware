/// \file data_rw.h
/// \brief File operations for loading vector map files, loading kml map files and writing log .csv files
/// \author Hatem Darweesh
/// \date Jun 23, 2016

#ifndef DATARW_H_
#define DATARW_H_

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <limits>

#include "vector_map_msgs/PointArray.h"
#include "vector_map_msgs/LaneArray.h"
#include "vector_map_msgs/NodeArray.h"
#include "vector_map_msgs/StopLineArray.h"
#include "vector_map_msgs/DTLaneArray.h"
#include "vector_map_msgs/LineArray.h"
#include "vector_map_msgs/AreaArray.h"
#include "vector_map_msgs/WayAreaArray.h"
#include "vector_map_msgs/SignalArray.h"
#include "vector_map_msgs/VectorArray.h"
#include "vector_map_msgs/CrossRoadArray.h"
#include "vector_map_msgs/RoadSignArray.h"
#include "vector_map_msgs/CurbArray.h"
#include "vector_map_msgs/RoadEdgeArray.h"
#include "vector_map_msgs/CrossWalkArray.h"

#include "utility.h"

namespace op_utility_ns
{

class DataRW
{
public:
  DataRW();
  virtual ~DataRW();

  static std::string LoggingMainfolderName;
  static std::string ControlLogFolderName;
  static std::string PathLogFolderName;
  static std::string GlobalPathLogFolderName;
  static std::string StatesLogFolderName;
  static std::string SimulationFolderName;
  static std::string KmlMapsFolderName;
  static std::string PredictionFolderName;
  static std::string TrackingFolderName;

  static void writeKMLFile(
    const std::string &fileName,
    const std::vector<std::string> &gps_list);
  static void writeKMLFile(
    const std::string &fileName,
    const std::vector<std::vector<std::string>> &gps_list);
  static void writeLogData(
    const std::string &logFolder,
    const std::string &logTitle, const std::string &header,
    const std::vector<std::string> &logData);
  static void createLoggingFolder();
};

class SimpleReaderBase
{
private:
  std::ifstream * m_pFile;
  std::vector<std::string> m_RawHeaders;
  std::vector<std::string> m_DataTitlesHeader;
  std::vector<std::vector<std::vector<std::string>>> m_AllData;
  int m_nHeders;
  int m_iDataTitles;
  int m_nVarPerObj;
  int m_nLineHeaders;
  std::string m_HeaderRepeatKey;
  char m_Separator;

  void readHeaders();
  void parseDataTitles(const std::string &header);

public:
  /**
   *
   * @param fileName log file name
   * @param nHeaders number of data headers
   * @param iDataTitles which row contains the data titles
   * @param nVariablesForOneObject 0 means each row represents one object
   */
  SimpleReaderBase(
    const std::string & fileName, const int & nHeaders = 2,
    const char & separator = ',', const int & iDataTitles = 1,
    const int & nVariablesForOneObject = 0, const int & nLineHeaders = 0,
    const std::string & headerRepeatKey = "...");

  ~SimpleReaderBase();

protected:
  int readAllData();
  bool readSingleLine(std::vector<std::vector<std::string>> &line);

};

class GPSDataReader : public SimpleReaderBase
{
public:
  struct GPSBasicData
  {
    double lat;
    double lon;
    double alt;
    double dir;
    double distance;

  };

public:
  GPSDataReader(const std::string & fileName)
  : SimpleReaderBase(fileName)
  {
  }
  ~GPSDataReader()
  {
  }

  bool readNextLine(GPSBasicData &data);
  int readAllData(std::vector<GPSBasicData> &data_list);
};

class SimulationFileReader : public SimpleReaderBase
{
public:
  struct SimulationPoint
  {
    double x;
    double y;
    double z;
    double a;
    double c;
    double v;
    std::string name;
  };

  struct SimulationData
  {
    SimulationPoint startPoint;
    SimulationPoint goalPoint;
    std::vector<SimulationPoint> simuCars;
  };

  SimulationFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
  }
  ~SimulationFileReader()
  {
  }

  bool readNextLine(SimulationPoint &data);
  int readAllData(SimulationData &data_list);
};

class LocalizationPathReader : public SimpleReaderBase
{
public:
  struct LocalizationWayPoint
  {
    double t;
    double x;
    double y;
    double z;
    double a;
    double v;
  };

  LocalizationPathReader(const std::string & fileName, const char & separator)
  : SimpleReaderBase(fileName, 1, separator)
  {
  }
  ~LocalizationPathReader()
  {
  }

  bool readNextLine(LocalizationWayPoint &data);
  int readAllData(std::vector<LocalizationWayPoint> &data_list);
};

class AisanPointsFileReader : public SimpleReaderBase
{
public:
  struct AisanPoints
  {
    int PID;
    double B;
    double L;
    double H;
    double Bx;
    double Ly;
    int Ref;
    int MCODE1;
    int MCODE2;
    int MCODE3;
  };

  AisanPointsFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }

  AisanPointsFileReader(const vector_map_msgs::PointArray & _points);
  ~AisanPointsFileReader()
  {
  }

  bool readNextLine(AisanPoints &data);
  int readAllData(std::vector<AisanPoints> &data_list);
  void parseNextLine(const vector_map_msgs::Point &_rec, AisanPoints &data);
  AisanPoints * getDataRowById(int _pid);
  std::vector<AisanPoints> m_data_list;

private:
  int m_min_id;
  std::vector<AisanPoints *> m_data_map;
};

class AisanNodesFileReader : public SimpleReaderBase
{
public:
  struct AisanNode
  {
    int NID;
    int PID;
  };

  AisanNodesFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }

  AisanNodesFileReader(const vector_map_msgs::NodeArray & _nodes);
  ~AisanNodesFileReader()
  {
  }

  bool readNextLine(AisanNode &data);
  int readAllData(std::vector<AisanNode> &data_list);
  void parseNextLine(const vector_map_msgs::Node &_rec, AisanNode &data);
  AisanNode * getDataRowById(int _nid);
  std::vector<AisanNode> m_data_list;

private:
  int m_min_id;
  std::vector<AisanNode *> m_data_map;
};

class AisanLinesFileReader : public SimpleReaderBase
{
public:
  struct AisanLine
  {
    int LID;
    int BPID;
    int FPID;
    int BLID;
    int FLID;
  };

  AisanLinesFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanLinesFileReader(const vector_map_msgs::LineArray & _lines);
  ~AisanLinesFileReader()
  {
  }

  bool readNextLine(AisanLine &data);
  int readAllData(std::vector<AisanLine> &data_list);
  void parseNextLine(const vector_map_msgs::Line &_rec, AisanLine &data);
  AisanLine * getDataRowById(int _lid);
  std::vector<AisanLine> m_data_list;

private:
  int m_min_id;
  std::vector<AisanLine *> m_data_map;
};

class AisanCenterLinesFileReader : public SimpleReaderBase
{
public:
  struct AisanCenterLine
  {
    int DID;
    int Dist;
    int PID;
    double Dir;
    double Apara;
    double r;
    double slope;
    double cant;
    double LW;
    double RW;
  };

  AisanCenterLinesFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanCenterLinesFileReader(const vector_map_msgs::DTLaneArray & _dtLanes);
  ~AisanCenterLinesFileReader()
  {
  }

  bool readNextLine(AisanCenterLine &data);
  int readAllData(std::vector<AisanCenterLine> &data_list);
  void parseNextLine(
    const vector_map_msgs::DTLane &_rec,
    AisanCenterLine &data);
  AisanCenterLine * getDataRowById(int _lnid);
  std::vector<AisanCenterLine> m_data_list;

private:
  int m_min_id;
  std::vector<AisanCenterLine *> m_data_map;
};

class AisanAreasFileReader : public SimpleReaderBase
{
public:
  struct AisanArea
  {
    int AID;
    int SLID;
    int ELID;
  };

  AisanAreasFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanAreasFileReader(const vector_map_msgs::AreaArray & _areas);
  ~AisanAreasFileReader()
  {
  }

  bool readNextLine(AisanArea &data);
  int readAllData(std::vector<AisanArea> &data_list);
  void parseNextLine(const vector_map_msgs::Area &_rec, AisanArea &data);
  AisanArea * getDataRowById(int _lnid);
  std::vector<AisanArea> m_data_list;

private:
  int m_min_id;
  std::vector<AisanArea *> m_data_map;
};

class AisanIntersectionFileReader : public SimpleReaderBase
{
public:
  struct AisanIntersection
  {
    int ID;
    int AID;
    int LinkID;
  };

  AisanIntersectionFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanIntersectionFileReader(const vector_map_msgs::CrossRoadArray & _inters);
  ~AisanIntersectionFileReader()
  {
  }

  bool readNextLine(AisanIntersection &data);
  int readAllData(std::vector<AisanIntersection> &data_list);
  void parseNextLine(
    const vector_map_msgs::CrossRoad &_rec,
    AisanIntersection &data);
  AisanIntersection * getDataRowById(int _lnid);
  std::vector<AisanIntersection> m_data_list;

private:
  int m_min_id;
  std::vector<AisanIntersection *> m_data_map;
};

class AisanLanesFileReader : public SimpleReaderBase
{
public:
  struct AisanLane
  {
    int LnID;
    int DID;
    int BLID;
    int FLID;
    int BNID;
    int FNID;
    int JCT;
    int BLID2;
    int BLID3;
    int BLID4;
    int FLID2;
    int FLID3;
    int FLID4;
    int ClossID;
    double Span;
    int LCnt;
    int Lno;
    int LaneType;
    int LimitVel;
    int RefVel;
    int RoadSecID;
    int LaneChgFG;
    int LinkWAID;
    char LaneDir;
    int LeftLaneId;
    int RightLaneId;

    int originalMapID;
  };

  AisanLanesFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanLanesFileReader(const vector_map_msgs::LaneArray & _lanes);
  ~AisanLanesFileReader()
  {
  }

  bool readNextLine(AisanLane &data);
  int readAllData(std::vector<AisanLane> &data_list);
  void parseNextLine(const vector_map_msgs::Lane &_rec, AisanLane &data);
  AisanLane * getDataRowById(int _lnid);
  std::vector<AisanLane> m_data_list;

private:
  int m_min_id;
  std::vector<AisanLane *> m_data_map;
};

class AisanStopLineFileReader : public SimpleReaderBase
{
public:
  struct AisanStopLine
  {
    int ID;
    int LID;
    int TLID;
    int SignID;
    int LinkID;
  };

  AisanStopLineFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanStopLineFileReader(const vector_map_msgs::StopLineArray & _stopLines);
  ~AisanStopLineFileReader()
  {
  }

  bool readNextLine(AisanStopLine &data);
  int readAllData(std::vector<AisanStopLine> &data_list);
  void parseNextLine(
    const vector_map_msgs::StopLine &_rec,
    AisanStopLine &data);
  AisanStopLine * getDataRowById(int _lnid);
  std::vector<AisanStopLine> m_data_list;

private:
  int m_min_id;
  std::vector<AisanStopLine *> m_data_map;
};

class AisanRoadSignFileReader : public SimpleReaderBase
{
public:
  struct AisanRoadSign
  {
    int ID;
    int VID;
    int PLID;
    int Type;
    int LinkID;
  };

  AisanRoadSignFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanRoadSignFileReader(const vector_map_msgs::RoadSignArray & _signs);
  ~AisanRoadSignFileReader()
  {
  }

  bool readNextLine(AisanRoadSign &data);
  int readAllData(std::vector<AisanRoadSign> &data_list);
  void parseNextLine(
    const vector_map_msgs::RoadSign &_rec,
    AisanRoadSign &data);
  AisanRoadSign * getDataRowById(int _lnid);
  std::vector<AisanRoadSign> m_data_list;

private:
  int m_min_id;
  std::vector<AisanRoadSign *> m_data_map;
};

class AisanSignalFileReader : public SimpleReaderBase
{
public:
  struct AisanSignal
  {
    int ID;
    int VID;
    int PLID;
    int Type;
    int LinkID;
  };

  AisanSignalFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanSignalFileReader(const vector_map_msgs::SignalArray & _signals);
  ~AisanSignalFileReader()
  {
  }

  bool readNextLine(AisanSignal &data);
  int readAllData(std::vector<AisanSignal> &data_list);
  void parseNextLine(const vector_map_msgs::Signal &_rec, AisanSignal &data);
  AisanSignal * getDataRowById(int _lnid);
  std::vector<AisanSignal> m_data_list;

private:
  int m_min_id;
  std::vector<AisanSignal *> m_data_map;
};

class AisanVectorFileReader : public SimpleReaderBase
{
public:
  struct AisanVector
  {
    int VID;
    int PID;
    double Hang;
    double Vang;
  };

  AisanVectorFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanVectorFileReader(const vector_map_msgs::VectorArray & _vectors);
  ~AisanVectorFileReader()
  {
  }

  bool readNextLine(AisanVector &data);
  int readAllData(std::vector<AisanVector> &data_list);
  void parseNextLine(const vector_map_msgs::Vector &_rec, AisanVector &data);
  AisanVector * getDataRowById(int _lnid);
  std::vector<AisanVector> m_data_list;

private:
  int m_min_id;
  std::vector<AisanVector *> m_data_map;
};

class AisanCurbFileReader : public SimpleReaderBase
{
public:
  struct AisanCurb
  {
    int ID;
    int LID;
    double Height;
    double Width;
    int dir;
    int LinkID;
  };

  AisanCurbFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanCurbFileReader(const vector_map_msgs::CurbArray & _curbs);
  ~AisanCurbFileReader()
  {
  }

  bool readNextLine(AisanCurb &data);
  int readAllData(std::vector<AisanCurb> &data_list);
  void parseNextLine(const vector_map_msgs::Curb &_rec, AisanCurb &data);
  AisanCurb * getDataRowById(int _lnid);
  std::vector<AisanCurb> m_data_list;

private:
  int m_min_id;
  std::vector<AisanCurb *> m_data_map;
};

class AisanRoadEdgeFileReader : public SimpleReaderBase
{
public:
  struct AisanRoadEdge
  {
    int ID;
    int LID;
    int LinkID;
  };

  AisanRoadEdgeFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanRoadEdgeFileReader(const vector_map_msgs::RoadEdgeArray & _roadEdges);
  ~AisanRoadEdgeFileReader()
  {
  }

  bool readNextLine(AisanRoadEdge &data);
  int readAllData(std::vector<AisanRoadEdge> &data_list);
  void parseNextLine(
    const vector_map_msgs::RoadEdge &_rec,
    AisanRoadEdge &data);
  AisanRoadEdge * getDataRowById(int _lnid);
  std::vector<AisanRoadEdge> m_data_list;

private:
  int m_min_id;
  std::vector<AisanRoadEdge *> m_data_map;
};

class AisanCrossWalkFileReader : public SimpleReaderBase
{
public:
  struct AisanCrossWalk
  {
    int ID;
    int AID;
    int Type;
    int BdID;
    int LinkID;
  };

  AisanCrossWalkFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanCrossWalkFileReader(
    const vector_map_msgs::CrossWalkArray & _crossWalks);
  ~AisanCrossWalkFileReader()
  {
  }

  bool readNextLine(AisanCrossWalk &data);
  int readAllData(std::vector<AisanCrossWalk> &data_list);
  void parseNextLine(
    const vector_map_msgs::CrossWalk &_rec,
    AisanCrossWalk &data);
  AisanCrossWalk * getDataRowById(int _lnid);
  std::vector<AisanCrossWalk> m_data_list;

private:
  int m_min_id;
  std::vector<AisanCrossWalk *> m_data_map;
};

class AisanWayareaFileReader : public SimpleReaderBase
{
public:
  struct AisanWayarea
  {
    int ID;
    int AID;
    int LinkID;
  };

  AisanWayareaFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
    m_min_id = std::numeric_limits<int>::max();
  }
  AisanWayareaFileReader(const vector_map_msgs::WayAreaArray & _wayArea);
  ~AisanWayareaFileReader()
  {
  }

  bool readNextLine(AisanWayarea &data);
  int readAllData(std::vector<AisanWayarea> &data_list);
  void parseNextLine(
    const vector_map_msgs::WayArea &_rec,
    AisanWayarea &data);
  AisanWayarea * getDataRowById(int _lnid);
  std::vector<AisanWayarea> m_data_list;

private:
  int m_min_id;
  std::vector<AisanWayarea *> m_data_map;
};

class AisanDataConnFileReader : public SimpleReaderBase
{
public:
  struct DataConn
  {
    int LID;             // lane id
    int SLID;             // stop line id
    int SID;             // signal id
    int SSID;             // stop sign id
  };

  AisanDataConnFileReader(const std::string & fileName)
  : SimpleReaderBase(fileName, 1)
  {
  }
  ~AisanDataConnFileReader()
  {
  }

  bool readNextLine(DataConn &data);
  int readAllData(std::vector<DataConn> &data_list);
};

class MapRaw
{
public:
  op_utility_ns::AisanLanesFileReader * pLanes;
  op_utility_ns::AisanPointsFileReader * pPoints;
  op_utility_ns::AisanCenterLinesFileReader * pCenterLines;
  op_utility_ns::AisanIntersectionFileReader * pIntersections;
  op_utility_ns::AisanAreasFileReader * pAreas;
  op_utility_ns::AisanLinesFileReader * pLines;
  op_utility_ns::AisanStopLineFileReader * pStopLines;
  op_utility_ns::AisanSignalFileReader * pSignals;
  op_utility_ns::AisanVectorFileReader * pVectors;
  op_utility_ns::AisanCurbFileReader * pCurbs;
  op_utility_ns::AisanRoadEdgeFileReader * pRoadedges;
  op_utility_ns::AisanWayareaFileReader * pWayAreas;
  op_utility_ns::AisanCrossWalkFileReader * pCrossWalks;
  op_utility_ns::AisanNodesFileReader * pNodes;

  struct timespec _time_out;

  MapRaw()
  {
    pLanes = nullptr;
    pPoints = nullptr;
    pCenterLines = nullptr;
    pIntersections = nullptr;
    pAreas = nullptr;
    pLines = nullptr;
    pStopLines = nullptr;
    pSignals = nullptr;
    pVectors = nullptr;
    pCurbs = nullptr;
    pRoadedges = nullptr;
    pWayAreas = nullptr;
    pCrossWalks = nullptr;
    pNodes = nullptr;

    UtilityH::getTickCount(_time_out);
  }

  ~MapRaw()
  {
    if (pLanes != nullptr) {
      delete pLanes;
      pLanes = nullptr;
    }

    if (pPoints != nullptr) {
      delete pPoints;
      pPoints = nullptr;
    }

    if (pCenterLines != nullptr) {
      delete pCenterLines;
      pCenterLines = nullptr;
    }

    if (pIntersections != nullptr) {
      delete pIntersections;
      pIntersections = nullptr;
    }

    if (pAreas != nullptr) {
      delete pAreas;
      pAreas = nullptr;
    }

    if (pLines != nullptr) {
      delete pLines;
      pLines = nullptr;
    }

    if (pStopLines != nullptr) {
      delete pStopLines;
      pStopLines = nullptr;
    }

    if (pSignals != nullptr) {
      delete pSignals;
      pSignals = nullptr;
    }

    if (pVectors != nullptr) {
      delete pVectors;
      pVectors = nullptr;
    }

    if (pCurbs != nullptr) {
      delete pCurbs;
      pCurbs = nullptr;
    }

    if (pRoadedges != nullptr) {
      delete pRoadedges;
      pRoadedges = nullptr;
    }

    if (pWayAreas != nullptr) {
      delete pWayAreas;
      pWayAreas = nullptr;
    }

    if (pCrossWalks != nullptr) {
      delete pCrossWalks;
      pCrossWalks = nullptr;
    }

    if (pNodes != nullptr) {
      delete pNodes;
      pNodes = nullptr;
    }
  }

  int getVersion()
  {
    bool bTimeOut = UtilityH::getTimeDiffNow(_time_out) > 2.0;
    bool bLoaded = pLanes != nullptr && pPoints != nullptr &&
      pCenterLines != nullptr && pNodes != nullptr;
    int iVersion = 0;
    if (bLoaded && bTimeOut) {
      iVersion = 2;
      if (pNodes->m_data_list.size() == 0) {
        iVersion = 1;
      }
    }
//		else
//		{
//			bLoaded =  pLanes != nullptr && pPoints != nullptr && pCenterLines  != nullptr;
//			if(bLoaded && bTimeOut)
//			{
//				iVersion = 1;
//				if(pNodes  == nullptr)
//					pNodes = new AisanNodesFileReader(vector_map_msgs::NodeArray());
//			}
//		}

    if (bLoaded && bTimeOut) {
      if (pIntersections == nullptr) {
        pIntersections = new AisanIntersectionFileReader(
          vector_map_msgs::CrossRoadArray());
      }

      if (pLines == nullptr) {
        pLines = new AisanLinesFileReader(vector_map_msgs::LineArray());
      }

      if (pStopLines == nullptr) {
        pStopLines = new AisanStopLineFileReader(
          vector_map_msgs::StopLineArray());
      }

      if (pSignals == nullptr) {
        pSignals = new AisanSignalFileReader(
          vector_map_msgs::SignalArray());
      }

      if (pVectors == nullptr) {
        pVectors = new AisanVectorFileReader(
          vector_map_msgs::VectorArray());
      }

      if (pCurbs == nullptr) {
        pCurbs = new AisanCurbFileReader(vector_map_msgs::CurbArray());
      }

      if (pRoadedges == nullptr) {
        pRoadedges = new AisanRoadEdgeFileReader(
          vector_map_msgs::RoadEdgeArray());
      }

      if (pWayAreas == nullptr) {
        pWayAreas = new AisanWayareaFileReader(
          vector_map_msgs::WayAreaArray());
      }

      if (pCrossWalks == nullptr) {
        pCrossWalks = new AisanCrossWalkFileReader(
          vector_map_msgs::CrossWalkArray());
      }
    }

    return iVersion;
  }
};

} /* namespace UtilityHNS */

#endif /* DATARW_H_ */
