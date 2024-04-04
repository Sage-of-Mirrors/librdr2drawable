#include "drawable/skeletondata.hpp"
#include "drawable/skeleton.hpp"
#include "util/bstream.h"


/* CJointData */

CJointData::CJointData() {

}

CJointData::~CJointData() {

}

void CJointData::Deserialize(bStream::CStream* stream) {
    mRotation.x = stream->readFloat();
    mRotation.y = stream->readFloat();
    mRotation.z = stream->readFloat();
    mRotation.w = stream->readFloat();

    mTranslation.x = stream->readFloat();
    mTranslation.y = stream->readFloat();
    mTranslation.z = stream->readFloat();
    mTranslation.w = stream->readFloat();

    mScale.x = stream->readFloat();
    mScale.y = stream->readFloat();
    mScale.z = stream->readFloat();
    mScale.w = stream->readFloat();

    uint64_t namePtr = stream->readUInt64() & 0x0FFFFFFF;
    size_t streamPos = stream->tell();
    
    stream->seek(namePtr);
    mName = stream->readString(256);
    stream->seek(streamPos);

    m0038 = stream->readUInt64();

    m0040 = stream->readUInt16();
    m0042 = stream->readUInt16();

    mNextSiblingIndex = stream->readUInt16();
    mParentIndex = stream->readUInt16();
    mCurrentIndex = stream->readUInt16();

    mTag = stream->readUInt16();

    m004C = stream->readUInt16();
    m004E = stream->readUInt16();
}

void CJointData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CJoint> CJointData::GetJoint() {
    std::shared_ptr<CJoint> joint = std::make_shared<CJoint>();

    joint->Name = mName.data();

    joint->Rotation = mRotation;

    joint->Translation.x = mTranslation.x;
    joint->Translation.y = mTranslation.y;
    joint->Translation.z = mTranslation.z;

    joint->Scale.x = mScale.x;
    joint->Scale.y = mScale.y;
    joint->Scale.z = mScale.z;

    return joint;
}


/* CSkeletonData */

CSkeletonData::CSkeletonData() {

}

CSkeletonData::~CSkeletonData() {

}

void CSkeletonData::Deserialize(bStream::CStream* stream) {
    size_t streamPos = 0;

    mVTable = stream->readUInt64();
    m0008 = stream->readUInt64();

    uint64_t tagsPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t tagsCount = stream->readUInt16();
    uint16_t tagsCapacity = stream->readUInt16();
    m001C = stream->readUInt32();

    uint32_t jointCountCopy = stream->readUInt32();
    uint32_t unk0024 = stream->readUInt32();

    uint64_t jointsPtr = stream->readUInt64() & 0x0FFFFFFF;

    uint64_t inverseBindPosesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t bindPosesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t indicesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t unk0048Ptr = stream->readUInt64() & 0x0FFFFFFF;

    uint32_t unk0050 = stream->readUInt32();
    uint32_t unk0054 = stream->readUInt32();
    uint32_t unk0058 = stream->readUInt32();

    uint16_t unk005C = stream->readUInt16();
    uint16_t jointCount = stream->readUInt16();

    uint8_t unk0060 = stream->readUInt8();
    uint8_t unk0061 = stream->readUInt8();
    mExternalParentJointTag = stream->readUInt16();

    uint32_t unk0064 = stream->readUInt32();
    uint64_t unk0068 = stream->readUInt64();

    stream->seek(indicesPtr);
    for (int i = 0; i < jointCount; i++) {
        mIndices.push_back(stream->readUInt16());
    }

    stream->seek(inverseBindPosesPtr);
    for (int i = 0; i < jointCount; i++) {
        Matrix4 mat;

        mat.r0.x = stream->readFloat();
        mat.r0.y = stream->readFloat();
        mat.r0.z = stream->readFloat();
        mat.r0.w = stream->readFloat();

        mat.r1.x = stream->readFloat();
        mat.r1.y = stream->readFloat();
        mat.r1.z = stream->readFloat();
        mat.r1.w = stream->readFloat();

        mat.r2.x = stream->readFloat();
        mat.r2.y = stream->readFloat();
        mat.r2.z = stream->readFloat();
        mat.r2.w = stream->readFloat();

        mat.r3.x = stream->readFloat();
        mat.r3.y = stream->readFloat();
        mat.r3.z = stream->readFloat();
        mat.r3.w = stream->readFloat();

        mInverseBindMatrices.push_back(mat);
    }

    stream->seek(bindPosesPtr);
    for (int i = 0; i < jointCount; i++) {
        Matrix4 mat;

        mat.r0.x = stream->readFloat();
        mat.r0.y = stream->readFloat();
        mat.r0.z = stream->readFloat();
        mat.r0.w = stream->readFloat();

        mat.r1.x = stream->readFloat();
        mat.r1.y = stream->readFloat();
        mat.r1.z = stream->readFloat();
        mat.r1.w = stream->readFloat();

        mat.r2.x = stream->readFloat();
        mat.r2.y = stream->readFloat();
        mat.r2.z = stream->readFloat();
        mat.r2.w = stream->readFloat();

        mat.r3.x = stream->readFloat();
        mat.r3.y = stream->readFloat();
        mat.r3.z = stream->readFloat();
        mat.r3.w = stream->readFloat();

        mBindMatrices.push_back(mat);
    }

    stream->seek(jointsPtr);
    for (int i = 0; i < jointCount; i++) {
        std::shared_ptr<CJointData> joint = std::make_shared<CJointData>();
        joint->Deserialize(stream);

        mJoints.push_back(joint);
    }
}

void CSkeletonData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CSkeleton> CSkeletonData::GetSkeleton() {
    std::shared_ptr<CSkeleton> skeleton = std::make_shared<CSkeleton>();

    for (int i = 0; i < mJoints.size(); i++) {
        std::shared_ptr<CJoint> joint = mJoints[i]->GetJoint();
        joint->InverseBindMatrix = mInverseBindMatrices[i];
        joint->BindMatrix = mBindMatrices[i];

        skeleton->FlatSkeleton.push_back(joint);
    }

    for (int i = 0; i < mJoints.size(); i++) {
        std::shared_ptr<CJointData> jointData = mJoints[i];
        std::shared_ptr<CJoint> joint = skeleton->FlatSkeleton[i];

        uint16_t parentIndex = jointData->GetParentIndex();
        if (parentIndex != 0xFFFF) {
            joint->Parent = skeleton->FlatSkeleton[parentIndex];
            joint->Parent.lock()->Children.push_back(joint);
        }
        else {
            joint->Parent;
            skeleton->Root = joint;
        }
    }

    return skeleton;
}
