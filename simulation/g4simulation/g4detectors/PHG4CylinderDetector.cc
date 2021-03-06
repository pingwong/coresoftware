#include "PHG4CylinderDetector.h"
#include "PHG4Parameters.h"

#include <g4main/PHG4PhenixDetector.h>
#include <g4main/PHG4Utils.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>

#include <Geant4/G4Colour.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4PhysicalConstants.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4VisAttributes.hh>

#include <cmath>
#include <sstream>

using namespace std;

//_______________________________________________________________
//note this inactive thickness is ~1.5% of a radiation length
PHG4CylinderDetector::PHG4CylinderDetector( PHCompositeNode *Node,  PHG4Parameters *parameters, const std::string &dnam, const int lyr ): 
  PHG4Detector(Node,dnam),
  params(parameters),
  cylinder_physi(NULL),
  layer(lyr)
{}

//_______________________________________________________________
bool PHG4CylinderDetector::IsInCylinder(const G4VPhysicalVolume * volume) const
{
  if (volume == cylinder_physi)
    {
      return true;
    }
  return false;
}


//_______________________________________________________________
void PHG4CylinderDetector::Construct( G4LogicalVolume* logicWorld )
{
  G4Material *TrackerMaterial = G4Material::GetMaterial(params->get_string_param("material"));

  if ( ! TrackerMaterial)
    {
      std::cout << "Error: Can not set material" << std::endl;
      exit(-1);
    }

  G4VisAttributes* siliconVis= new G4VisAttributes();
  if (params->get_int_param("blackhole"))
    {
      PHG4Utils::SetColour(siliconVis, "BlackHole");
      siliconVis->SetVisibility(false);
      siliconVis->SetForceSolid(false);
    }
  else
    {
      PHG4Utils::SetColour(siliconVis, params->get_string_param("material"));
      siliconVis->SetVisibility(true);
      siliconVis->SetForceSolid(true);
    }

  // determine length of cylinder using PHENIX's rapidity coverage if flag is true
  double radius = params->get_double_param("radius")*cm;
  double thickness = params->get_double_param("thickness")*cm;
  G4VSolid *cylinder_solid = new G4Tubs(G4String(GetName().c_str()),
			      radius,
			      radius + thickness, 
			      params->get_double_param("length")*cm/2. ,0,twopi);
  G4LogicalVolume *cylinder_logic = new G4LogicalVolume(cylinder_solid, 
				       TrackerMaterial, 
				       G4String(GetName().c_str()),
				       0,0,0);
  cylinder_logic->SetVisAttributes(siliconVis);
  cylinder_physi = new G4PVPlacement(0, G4ThreeVector(params->get_double_param("place_x")*cm,
                                                      params->get_double_param("place_y")*cm,
						      params->get_double_param("place_z")*cm), 
				     cylinder_logic, 
				     G4String(GetName().c_str()), 
				     logicWorld, 0, false, overlapcheck);
}
