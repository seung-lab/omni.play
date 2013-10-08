#pragma once

#include <QDataStream>

class OmSegmentation;

class OmMetadataSegmentation {
 private:
  const int fileVersion_;

 public:
  OmMetadataSegmentation(const int fileVersion) : fileVersion_(fileVersion) {}

  //	void Read(QDataStream& in, OmSegmentation& seg);
  //	void Write(QDataStream& out, const OmSegmentation& seg);
};
