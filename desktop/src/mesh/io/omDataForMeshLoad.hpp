#pragma once

#include "common/omCommon.h"

class OmDataForMeshLoad{
public:
    OmDataForMeshLoad()
        : hasData_(false)
        , vertexIndexCount_(0)
        , vertexIndexNumBytes_(0)
        , stripDataCount_(0)
        , stripDataNumBytes_(0)
        , vertexDataCount_(0)
        , vertexDataNumBytes_(0)
        , trianDataCount_(0)
        , trianDataNumBytes_(0)
    {}

    inline bool HasData() const {
        return hasData_;
    }

    inline void HasData(const bool val){
        hasData_ = val;
    }

    uint64_t NumBytes() const
    {
        return vertexIndexNumBytes_ +
            vertexDataNumBytes_ +
            stripDataNumBytes_ +
            trianDataNumBytes_;
    }

// vertex index data
    inline uint32_t* VertexIndex() const {
        return vertexIndex_.get();
    }

    inline om::shared_ptr<uint32_t> VertexIndexSharedPtr() const {
        return vertexIndex_;
    }

    inline uint32_t VertexIndexCount() const {
        return vertexIndexCount_;
    }

    inline uint64_t VertexIndexNumBytes() const {
        return vertexIndexNumBytes_;
    }

    void SetVertexIndex(om::shared_ptr<uint32_t> vertexIndex,
                        const uint32_t vertexIndexCount,
                        const uint64_t numBytes)
    {
        vertexIndex_ = vertexIndex;
        vertexIndexCount_ = vertexIndexCount;
        vertexIndexNumBytes_ = numBytes;
    }

// strip data
    inline uint32_t* StripData() const {
        return stripData_.get();
    }

    inline om::shared_ptr<uint32_t> StripDataSharedPtr() const {
        return stripData_;
    }

    inline uint32_t StripDataCount() const {
        return stripDataCount_;
    }

    inline uint64_t StripDataNumBytes() const {
        return stripDataNumBytes_;
    }

    void SetStripData(om::shared_ptr<uint32_t> stripData,
                      const uint32_t stripDataCount,
                      const uint64_t numBytes)
    {
        stripData_ = stripData;
        stripDataCount_ = stripDataCount;
        stripDataNumBytes_ = numBytes;
    }

//  vertex data
    inline float* VertexData() const {
        return vertexData_.get();
    }

    inline om::shared_ptr<float> VertexDataSharedPtr() const {
        return vertexData_;
    }

    inline uint32_t VertexDataCount() const {
        return vertexDataCount_;
    }

    inline uint64_t VertexDataNumBytes() const {
        return vertexDataNumBytes_;
    }

    void SetVertexData(om::shared_ptr<float> vertexData,
                       const uint32_t vertexDataCount,
                       const uint64_t numBytes)
    {
        vertexData_ = vertexData;
        vertexDataCount_ = vertexDataCount;
        vertexDataNumBytes_ = numBytes;
    }

// triangle data
    inline uint32_t* TrianData() const {
        return trianData_.get();
    }

    inline om::shared_ptr<uint32_t> TrianDataSharedPtr() const {
        return trianData_;
    }

    inline uint32_t TrianDataCount() const {
        return trianDataCount_;
    }

    inline uint64_t TrianDataNumBytes() const {
        return trianDataNumBytes_;
    }

    void SetTrianData(om::shared_ptr<uint32_t> trianData,
                      const uint32_t trianDataCount,
                      const uint64_t numBytes)
    {
        trianData_ = trianData;
        trianDataCount_ = trianDataCount;
        trianDataNumBytes_ = numBytes;
    }

private:
    bool hasData_;

    om::shared_ptr<uint32_t> vertexIndex_;
    uint32_t vertexIndexCount_;
    uint64_t vertexIndexNumBytes_;

    om::shared_ptr<uint32_t> stripData_;
    uint32_t stripDataCount_;
    uint64_t stripDataNumBytes_;

    om::shared_ptr<float> vertexData_;
    uint32_t vertexDataCount_;
    uint64_t vertexDataNumBytes_;

    om::shared_ptr<uint32_t> trianData_;
    uint32_t trianDataCount_;
    uint64_t trianDataNumBytes_;
};

inline std::ostream& operator<<(std::ostream& out, const OmDataForMeshLoad& dfml)
{
	out << "HasData: " << dfml.HasData() << std::endl;
	out << "VertexIndex: " << dfml.VertexIndex() << std::endl;
	out << "VertexIndexCount: " << dfml.VertexIndexCount() << std::endl;
	out << "VertexIndexNumBytes: " << dfml.VertexIndexNumBytes() << std::endl;
	out << "StripData: " << dfml.StripData() << std::endl;
	out << "StripDataCount: " << dfml.StripDataCount() << std::endl;
	out << "StripDataNumBytes: " << dfml.StripDataNumBytes() << std::endl;
	out << "VertexData: " << dfml.VertexData() << std::endl;
	out << "VertexDataCount: " << dfml.VertexDataCount() << std::endl;
	out << "VertexDataNumBytes: " << dfml.VertexDataNumBytes() << std::endl;
	out << "TrianData: " << dfml.TrianData() << std::endl;
	out << "TrianDataCount: " << dfml.TrianDataCount() << std::endl;
	out << "TrianDataNumBytes: " << dfml.TrianDataNumBytes() << std::endl;
    return out;
}

