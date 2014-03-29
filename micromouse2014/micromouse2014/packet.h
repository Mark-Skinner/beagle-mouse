//
//  packet.h
//  Test
//
//  Created by Lyle Moffitt on 3/7/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef Test_packet_h
#define Test_packet_h

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <map>

// #include <typeinfo>

struct data;
struct packet;

typedef std::chrono::time_point<std::chrono::system_clock>::rep timestamp;

/// Specific hard-coded values for the xv-11 LIDAR packet
namespace packet_config
{
    /// The smallest valid value for packet.index
    constexpr uint index_min        = 0xA0;
    /// The largest valid value for packet.index
    constexpr uint index_max        = 0xF9;
    /// The number of packets created in a rotation
    constexpr uint index_range      = 90;
    /// The number of data points in a single packet
    constexpr uint data_num         = 4;
};

/** The 4-Byte packet sub-component with primary data.
 
 It is organized Least Significant BYTE (LSB) first, but each byte is 
 organized Most Significant *bit* (MSb) first.
 
 The Byte sequence is:
    B0 :    <distance [7:0]>
    B1 :    <"invalid data" flag [15]> 
            <"strength warning" flag [16]>
            <distance [13:8]>
    B2 :    <signal strength [7:0]>
    B3 :    <signal strength [15:8]>
 */
struct data
{
    /// Preferred constructor
    data(packet * _p, std::vector<uint8_t> _d);
    
    /// Copy constructor, if necessary
    data(data & _d);
    
    /// The distance to the point of reflection measured in milimeters.
    const uint16_t      distance;        
    
    /** Warning when the reported strength is greatly inferior to what
        is expected at this distance.
     
     Possible causes:
        * Material is too dark.
        * Incomplete surface (edge, corner, too bumpy).
        * Material is translucent.
     */
    const bool          strength_warn;    
    
    /** Indicates that the distance could not be calculated.
     
     When triggered, 'distance' will be an error code, for ex:
        0x02, 0x03, 0x21, 0x25, 0x35 or 0x50...
     */
    const bool          invalid_data;     
    
    /// Strength of the signal
    const uint16_t      strength;         //31 : 16
    
    /// Pointer to the containing packet
    const packet *      source;
    
    int  eval_dist(){   return (invalid_data?-1:distance); }
    void print(uint index) const;
    
 };


/** The full 22-Byte packet received from the lidar.
 
 It is organized Least Significant BYTE (LSB) first, but each byte is 
 organized Most Significant *bit* (MSb) first.
 
 Orgnaized as:
    B{0-1}  : <start> <index> 
    B{2-3}  : <speed_L> <speed_H> 
    B{...}  : [Data0 {4-7}] [Data1 {8-11}] [Data2 {12-15}] [Data3 {16-19}]
    B{20-21}: <checksum_L> <checksum_H>
 */
struct packet
{
    /// Packet constructor
    packet(std::vector<uint8_t> _v);
    
    /// The time the packet was constructed, as measured by the OS
    const timestamp p_time;
    
    /// The Start-byte (always 0xFA)
    const uint8_t    start;
    
    /** The packet index number out of 90 total.
     
     Literally enumerated {0xA0 - 0xF9}, representing #0 - #89.
     */
    const uint8_t    index;
    
    /// The rotational speed of the lidar measured as 64th of RPM 
    uint16_t      speed;
    
    /// The primary data information of the packet.
    const data       d0,d1,d2,d3;
    
    /// The *contained* checksum value
    const uint16_t    chksum;// == chkcalc (?)
    
    /// A checksum calculated from the received information.
    uint16_t    chkcalc;// == chksum (?)
    
    /// Average distance of the good distance measures.
    
    double     avg_dist;
    
    void print() const;
    
};



/** A list of DATA points organized radially by degree {0-359}.
 */
struct _360_scan
{
    ///Stores pointer to each data sub-packet composing the 360 degrees
    std::map<uint,const data*> deg_index;
    
    ///The times that degree 0 and 359 were entered (respectively)
    timestamp   begin,end;
    
    ///deconstructs the packet and stores it in the map.
    size_t add_pkt(packet* _p);
    
    ///Print out the list. Template defines whether packet or data(default)
    
    void print_pkt();
    void print_data();
};





#endif