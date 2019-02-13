//
// Created by Christian Kater on 20.01.19.
//

#ifndef DCPLIB_DCPSLAVEWRITER_HPP
#define DCPLIB_DCPSLAVEWRITER_HPP

#include <zip.h>
#include <dcp/xml/DcpSlaveDescriptionWriter.hpp>

static void writeDcpSlaveFile(std::shared_ptr<SlaveDescription_t> slaveDescription, std::string dcpZipFile) {
    std::string file     = "v" + std::to_string(slaveDescription->dcpMajorVersion) + "." + std::to_string(slaveDescription->dcpMinorVersion) + "/dcpSlaveDescription.dcpx";
    std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + to_string(*slaveDescription);


    int error = 0;
    zip * zip = zip_open(dcpZipFile.c_str(), ZIP_CREATE, &error);
    if(zip == nullptr) {
        throw std::runtime_error("could not create zip file");
    }

    const char *content = xml.c_str();
    zip_source *buffer = zip_source_buffer(zip, content, xml.length(), 0);
    if(buffer == nullptr) {
        throw std::runtime_error("could not create buffer for slave description");
    }

    int index = (int)zip_file_add(zip, file.c_str(), buffer, ZIP_FL_OVERWRITE);
    if(index < 0)
    {
        throw std::runtime_error("could not add slave description to zip file");
    }
    zip_close(zip);
}

#endif //DCPLIB_DCPSLAVEWRITER_HPP
