/*
* Copyright 2016 Devan Lai
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef DFU_H
#define DFU_H

enum DFURequest {
    DFU_DETACH,
    DFU_DNLOAD,
    DFU_UPLOAD,
    DFU_GETSTATUS,
    DFU_CLRSTATUS,
    DFU_GETSTATE,
    DFU_ABORT,
};

enum DFUClass {
    DFU_CLASS_APP_SPECIFIC = 0xFE,
};

enum DFUSubClass {
    DFU_SUBCLASS_DFU = 0x01,
};

enum DFUProtocol {
    DFU_PROTO_RUNTIME = 0x01,
    DFU_PROTO_DFU = 0x02
};

enum DFUDescriptorType {
    DFU_DESCRIPTOR = 0x21,
};

enum DFUAttributes {
    DFU_ATTR_CAN_DOWNLOAD = 0x01,
    DFU_ATTR_CAN_UPLOAD = 0x02,
    DFU_ATTR_MANIFEST_TOLERANT = 0x04,
    DFU_ATTR_WILL_DETACH = 0x08,
};

enum DFUVersion {
    DFU_VERSION_1_00 = 0x0100,
    DFUSE_VERSION_1_1A = 0x011A,
};

#endif