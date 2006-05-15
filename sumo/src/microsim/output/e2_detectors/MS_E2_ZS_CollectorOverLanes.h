#ifndef MS_E2_ZS_CollectorOverLanes_h
#define MS_E2_ZS_CollectorOverLanes_h
//---------------------------------------------------------------------------//
//                        MS_E2_ZS_CollectorOverLanes.h -
//  A detector which joins E2Collectors over consecutive lanes (backward)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.11  2006/05/15 05:47:50  dkrajzew
// got rid of the cell-to-meter conversions
//
// Revision 1.11  2006/05/08 10:54:42  dkrajzew
// got rid of the cell-to-meter conversions
//
// Revision 1.10  2006/04/11 10:59:06  dkrajzew
// all structures now return their id via getID()
//
// Revision 1.9  2005/11/09 06:36:06  dkrajzew
// changing the LSA-API: MSEdgeContinuation added
//
// Revision 1.8  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:08:20  dkrajzew
// LARGE CODE RECHECK
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSE2Collector.h"
#include <utils/iodevices/XMLDevice.h>



class MSEdgeContinuations;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MS_E2_ZS_CollectorOverLanes
 * This class is somekind of a wrapper over several MSE2Collectors.
 * For some reasons it may be necessary to use MSE2Collectors that are
 *  longer than the lane they begin at. In this case, this class should be
 *  used. MSE2Collectors are laid on consecutive lanes backwards, building
 *  a virtual detector for each lane combination.
 * There are still some problems with it: we do not know how the different
 *  combinations shall be treated. Really verified is only the
 *  CURRENT_HALTING_DURATION_SUM_PER_VEHICLE-detector.
 */
class MS_E2_ZS_CollectorOverLanes :
    public MSDetectorFileOutput
{
public:
	/// Definition of a E2-collector storage
    typedef std::vector< MSE2Collector* > CollectorCont;

	/// Constructor
    MS_E2_ZS_CollectorOverLanes( std::string id,
        DetectorUsage usage, MSLane* lane, SUMOReal startPos,
        SUMOReal haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

	/** @brief Builds the consecutive E2-detectors
		This is not done within the constructor to allow overriding of
		most functions but the building of detectors itself which in fact
		is depending on whether the normal or the gui-version is used */
    void init(MSLane *lane, SUMOReal detLength,
        const MSEdgeContinuations &edgeContinuations);

	/// Destructor
    virtual ~MS_E2_ZS_CollectorOverLanes( void );

	/// Returns this detector's current value for the measure of the given type
    SUMOReal getCurrent( E2::DetType type );

	/// Returns this detector's aggregated value for the given measure
    SUMOReal getAggregate( E2::DetType type, SUMOReal lastNSeconds );

	/// Returns the information whether the given type is computed
    bool hasDetector( E2::DetType type );

	/// Adds the measure of the given type
    void addDetector( E2::DetType type, std::string detId = "" );

	/// Returns this detector's id
    const std::string &getID() const;

	/// Returns the id of the lane this detector starts at
    const std::string &getStartLaneID() const;

	/// ... have to override this method
    void resetQueueLengthAheadOfTrafficLights( void );


    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     */
    //@{
    /**
     * Returns a string indentifying an object of this class. Used for
     * distinct filenames.
     */
    std::string  getNamePrefix( void ) const
        {
            return "MS_E2_ZS_CollectorOverLanes";
        }

    /**
     * Get a header for file output which shall contain some
     * explanation of the output generated by getXMLOutput.
     */
    void writeXMLHeader( XMLDevice &dev ) const
        {
            dev.writeString(xmlHeaderM);
        }

    /**
     * Get the XML-formatted output of the concrete detector.
     *
     * @param lastNTimesteps Generate data out of the interval
     * (now-lastNTimesteps, now].
     */
    void writeXMLOutput( XMLDevice &dev,
        SUMOTime startTime, SUMOTime stopTime );

    /**
     * Get an opening XML-element containing information about the detector.
     */
    void writeXMLDetectorInfoStart( XMLDevice &dev ) const;

    void writeXMLDetectorInfoEnd( XMLDevice &dev ) const
    {
        dev.writeString(infoEndM);
    }

    /**
     * Get the data-clean up interval in timesteps.
     */
    SUMOTime getDataCleanUpSteps( void ) const
        {
            return deleteDataAfterSecondsM; // !!! Konvertierung
        }
    //@}

	/// Returns this detector's length
    SUMOReal getLength() const {
        return myLength;
    }

protected:
	/** @brief This method extends the current length up to the given
		This method is called consecutively until all paths have the
		desired length */
    void extendTo(SUMOReal length,
		const MSEdgeContinuations &edgeContinuations);

	/// Builds an id for one of the E2-collectors this detector uses
    std::string  makeID( const std::string &baseID,
        size_t c, size_t r ) const;

	/// Builds a single E2-collector
    virtual MSE2Collector *buildCollector(size_t c, size_t r,
        MSLane *l, SUMOReal start, SUMOReal end);


	std::vector<MSLane*> getLanePredeccessorLanes(MSLane *l,
		const MSEdgeContinuations &edgeContinuations);

protected:
	/// The position the collector starts at
    SUMOReal startPosM;

	/// The length of the collector
    SUMOReal myLength;

	/// The information for how many seconds data shall be saved
    SUMOTime deleteDataAfterSecondsM;

	/// Describes how long a vehicle shall stay before being assigned to a jam
    MSUnit::Steps haltingTimeThresholdM;

	/// Describes how slow a vehicle must be before being assigned to a jam
    MSUnit::CellsPerStep haltingSpeedThresholdM;

	/// Describes how long a jam must be before being recognized
    SUMOReal jamDistThresholdM;

	/// Definition of a lane storage
    typedef std::vector<MSLane*> LaneVector;

	/// Definition of a storage for lane vectors
    typedef std::vector<LaneVector> LaneVectorVector;

	/// Definition of a detector storage
    typedef std::vector<MSE2Collector*> DetectorVector;

	/// Definition of astorage for detector vectors
    typedef std::vector<DetectorVector> DetectorVectorVector;

	/// Definition of a SUMOReal storage
    typedef std::vector<SUMOReal> DoubleVector;

	/// Definition of a storage for SUMOReal vectors
    typedef DoubleVector LengthVector;

	/** @brief Storage for lane combinations
		Each lane combination is a vector of consecutive lanes (backwards) */
    LaneVectorVector myLaneCombinations;

	/** @brief Storage for detector combinations
		Each detector combination is a vector of consecutive lanes (backwards) */
    DetectorVectorVector myDetectorCombinations;
	/** @brief Storage for length combinations
		Each length combination is a vector of consecutive lanes (backwards) */
    LengthVector myLengths;

	/// The string that is printed in front of the output file
    static std::string xmlHeaderM;

	/// The id of this detector
    std::string myID;

	/// The id of the lane this detector starts at
    std::string myStartLaneID;

	/// Definition of a map from a lane to the detector lying on it
    typedef std::map<MSLane*, MSE2Collector*> LaneDetMap;

	/// Storage for detectors which already have been build for a single lane
    LaneDetMap myAlreadyBuild;

    DetectorUsage myUsage;

private:
    static std::string infoEndM;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

