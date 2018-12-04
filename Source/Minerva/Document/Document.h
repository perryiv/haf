
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_DOCUMENT_CLASS_H_
#define _MINERVA_DOCUMENT_CLASS_H_

#include "Minerva/Document/Body.h"
#include "Minerva/Document/CompileGuard.h"

#include "SceneGraph/Visitors/Visitor.h"
#include "SceneGraph/Nodes/Groups/Group.h"

#include "Usul/Atomic/Object.h"
#include "Usul/Documents/FileDocument.h"
#include "Usul/Interfaces/IViewCreate.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector4.h"

#include "boost/tuple/tuple.hpp"


namespace Minerva {


class Document : 
  public Usul::Documents::FileDocument,
  public Usul::Interfaces::IViewCreate
{
public:

  typedef Usul::Documents::FileDocument BaseClass;
  typedef SceneGraph::Visitors::Visitor SceneVisitor;
  typedef SceneGraph::Nodes::Groups::Group Group;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Math::Vec2i Vec2i;
  typedef Usul::Math::Matrix44d Matrix;
  typedef boost::tuples::tuple < Vec2i, Vec2i, Vec2i, bool, bool, bool, Matrix > MouseEventData;
  typedef Usul::Threads::Guard < Usul::Threads::Mutex > Guard;
  typedef Usul::Math::Vec4f Vec4f;
  typedef Usul::Math::Vector2 < Vec4f > BackgroundColors;

  USUL_REFERENCED_CLASS ( Document );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction
  Document ( const std::string &file = std::string() );

  // Accept the scene visitor.
  void              accept ( SceneVisitor & );

  // Get the background color for the navigation matrix.
  Vec4f             backgroundColor ( const Matrix &m ) const;

  // Clear the document.
  virtual void      clearObject();

  // Usul::Interfaces::IViewCreate
  virtual bool      canCreateView ( IUnknown::RefPtr document ) const;
  virtual void      createView ( QWidget *parent, IUnknown::RefPtr document, IUnknown::RefPtr caller );

  // Destroy the document.
  virtual void      destroyObject();

  // Handle mouse event and update internal members.
  void              handleMouseMove ( const MouseEventData & );

  // Calculate the navigation matrix.
  Matrix            navigationMatrix() const;

  // Reset the internal members related to navigation.
  void              navigationReset();

  // Functions for working with the zoom.
  void              zoomClamp();
  double            zoomGet() const;
  void              zoomSet ( double );
  void              zoomFraction ( double );

protected:

  // Use reference counting.
  virtual ~Document();

  Matrix            _calculatePitchMatrix() const;

  void              _updatePitch    ( const MouseEventData &, const Matrix & );
  void              _updateRotation ( const MouseEventData &, const Matrix & );
  void              _updateZoom     ( const MouseEventData &, const Matrix & );

private:

  void              _destroy();

  Usul::Atomic::Object < Group::RefPtr > _root;
  Usul::Atomic::Object < Body::RefPtr > _body;
  Usul::Atomic::Object < double > _pitchAngle;
  Usul::Atomic::Object < double > _pitchArmLength;
  Usul::Atomic::Object < double > _zTranslation;
  Usul::Atomic::Object < Matrix > _rotation;
  Usul::Atomic::Object < BackgroundColors > _backgroundColors;
};


} // namespace Minerva


#endif // _MINERVA_DOCUMENT_CLASS_H_
