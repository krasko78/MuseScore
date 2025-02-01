/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MUSE_GLOBAL_ZIPWRITER_H
#define MUSE_GLOBAL_ZIPWRITER_H

#include "io/path.h"
#include "io/iodevice.h"
#include "types/errorcallback.h"

namespace muse {
class ZipWriter
{
public:

    explicit ZipWriter(const io::path_t& filePath, ErrorCallback onErrorCallback = nullptr);
    explicit ZipWriter(io::IODevice* device, ErrorCallback onErrorCallback = nullptr);
    ~ZipWriter();

    void close();
    bool hasError() const;

    bool addFile(const std::string& fileName, const ByteArray& data);

private:

    void onError(int error, const std::string& errorString);
    void flush();

    struct Impl;
    Impl* m_impl = nullptr;
    io::IODevice* m_device = nullptr;
    bool m_selfDevice = false;
    bool m_hasError = false;
    ErrorCallback m_onError;
};
}

#endif // MUSE_GLOBAL_ZIPWRITER_H
