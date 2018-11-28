# DCPLib #

DCPLib is a C++ implementation of the Distributed Co-Simulation Protocol (DCP). It provides an API for slaves (e. g. simulators) and master tools. 

## Packages ##
| Package          | Description | Dependencies                           |
|------------------|-------------|----------------------------------------|
| DCPLib::core     | Containing all common classes, like constants, PDU definitions etc.            |                                        |
| DCPLib::master   | Containing all classes relevant to build a master tool for DCP            | DCPLib::core                           |
| DCPLib::slave    | Containing all classes relevant to implemant an DCP slave.             | DCPLib::core                           |
| DCPLib::Ethernet | Classes to add UDP_IPv4 or TCP support to the DCLib::master or DCPLib::slave package            | Asio standalone, DCPLib::core, Threads |
## Wiki ##
For hints how to use this library, take a look at the [wiki](https://github.com/ChKater/DCPLib/wiki) pages
## Example ##
See [example](example) for a implementation of a master and slave.

## Acknowledgement ##
- 2016 - 2018: The work on this library was done in the contex of the ITEA3 Project ACOSAR (N◦14004). It was partially funded by the Austrian Competence Centers for Excellent Technologies (COMET) program, the Austrian Research Promotion Agency (FFG), and by the German Federal Ministry of Education and Research (BMBF) under the support code 01lS15033A.
