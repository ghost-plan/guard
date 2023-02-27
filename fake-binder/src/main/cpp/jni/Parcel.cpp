#include "Parcel.h"

namespace jamesfchen {
Parcel::Parcel() {}

Parcel::~Parcel() {

}
void Parcel::registerNatives() {
  registerHybrid({
					 makeNativeMethod("initHybrid", Parcel::initHybrid),
				 });
}
void Parcel::initHybrid(facebook::jni::alias_ref<jhybridobject> o) {
  // The arguments will be forwarded to the ctor, and the result
  // will be saved in mHybridPointer in the java object.
  setCxxInstance(o);
}
const uint8_t *Parcel::data() const {
  return NULL;
}

size_t Parcel::dataSize() const {
  return NULL;
}

size_t Parcel::dataAvail() const {
  return NULL;
}

size_t Parcel::dataPosition() const {
  return NULL;
}

size_t Parcel::dataCapacity() const {
  return NULL;
}

int32_t Parcel::setDataSize(size_t size) {
  return NULL;
}

void Parcel::setDataPosition(size_t pos) const {

}

int32_t Parcel::setDataCapacity(size_t size) {
  return NULL;
}

bool Parcel::hasFileDescriptors() const {
  return false;
}

bool Parcel::checkInterface(IBinder *) const {
  return false;
}

void Parcel::freeData() {

}

const size_t *Parcel::objects() const {
  return NULL;
}

size_t Parcel::objectsCount() const {
  return NULL;
}

int32_t Parcel::errorCheck() const {
  return NULL;
}

void Parcel::setError(int32_t err) {

}

int32_t Parcel::write(const void *data, size_t len) {
  return NULL;
}

void *Parcel::writeInplace(size_t len) {
  return NULL;
}

int32_t Parcel::writeInt32(int32_t val) {
  return NULL;
}

int32_t Parcel::writeInt64(int64_t val) {
  return NULL;
}

int32_t Parcel::writeFloat(float val) {
  return NULL;
}

int32_t Parcel::writeDouble(double val) {
  return NULL;
}

int32_t Parcel::writeIntPtr(intptr_t val) {
  return NULL;
}

int32_t Parcel::writeCString(const char *str) {
  return NULL;
}

int32_t Parcel::writeString8(const String8 &str) {
  return NULL;
}

int32_t Parcel::writeString16(const String16 &str) {
  return NULL;
}

int32_t Parcel::writeString16(const char16_t *str, size_t len) {
  return NULL;
}

int32_t Parcel::writeStrongBinder(const IBinder &val) {
  return NULL;
}

int32_t Parcel::writeWeakBinder(const IBinder &val) {
  return NULL;
}

void Parcel::remove(size_t start, size_t amt) {

}

int32_t Parcel::read(void *outData, size_t len) const {
  return NULL;
}

const void *Parcel::readInplace(size_t len) const {
  return NULL;
}

int32_t Parcel::readInt32() const {
  return NULL;
}

int64_t Parcel::readInt64() const {
  return NULL;
}

float Parcel::readFloat() const {
  return 0;
}

double Parcel::readDouble() const {
  return 0;
}

intptr_t Parcel::readIntPtr() const {
  return NULL;
}

const char *Parcel::readCString() const {
  return NULL;
}

String8 Parcel::readString8() const {
  return String8();
}

const char16_t *Parcel::readString16Inplace(size_t *outLen) const {
  return NULL;
}

int32_t Parcel::readExceptionCode() const {
  return NULL;
}

int Parcel::readFileDescriptor() const {
  return 0;
}

const flat_binder_object *Parcel::readObject(bool nullMetaData) const {
  return NULL;
}

void Parcel::closeFileDescriptors() {

}

const uint8_t *Parcel::ipcData() const {
  return NULL;
}

size_t Parcel::ipcDataSize() const {
  return NULL;
}

const size_t *Parcel::ipcObjects() const {
  return NULL;
}

size_t Parcel::ipcObjectsCount() const {
  return NULL;
}

void Parcel::print(TextOutput &to, uint32_t flags) const {

}

void Parcel::releaseObjects() {

}

void Parcel::acquireObjects() {

}

void Parcel::freeDataNoInit() {

}

void Parcel::initState() {

}

void Parcel::scanForFds() const {

}

template<class T>
T Parcel::readAligned() const {
  return NULL;
}

void Parcel::ipcSetDataReference(const uint8_t *data,
								 size_t dataSize,
								 const size_t *objects,
								 size_t objectsCount,
								 release_func relFunc,
								 void *relCookie) {

}

}