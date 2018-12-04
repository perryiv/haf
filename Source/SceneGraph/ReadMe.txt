-- Need to automatically set near and far clipping planes.


-- Need to code LightModel the same way you did Light, especially HSG parsing.
-- Add Hint to HSG parser.
-- Need a <state_store> or <states> element in HSG format that works like 
   instances. Define at top of file.
-- Viewer needs a state container that it visits before drawing.
-- Can you eliminate the "LIGHT0" enumerations and just use an int member?


-- Dropped the "effect" abstraction and moved the Shaders in to the StateSet.
-- Sorting with StateSet pointers now.


-- Decided to use the "effect" abstraction.
-- Each shape will have an effect.
-- Effects can be whatever... fixed functionality, shader-based, or some combo.
-- Author the effects with XML and a few small building block classes
   like Lighting, Material, Color, LineWidth, etc.
-- This will replace the StateSet and AttributesMap.
-- Need a name-based lookup of Effect classes and the ability to 
   register them at runtime.
-- Read the XML files at startup like the shader files.
-- Have effect class implement virtual "compare()" function for sorting.


Viewer contains
  _updatePipeline .... Vector of UpdateVisitor::RefPtr called from update()
  _cullPipeline ...... Vector of CullVisitor::RefPtr called from render()
  _drawPipeline ...... Vector of DrawVisitor::RefPtr
  _scene ............. User sets this
  _renderLists ....... Cull and draw visitors rebuild this each frame

Visitor
  UpdateVisitor ...... Pass _scene
  CullVisitor ........ Pass _scene, _renderList, and matrices
    FrustumCullVisitor
  DrawMethod ......... Pass _renderList after sorting
    DrawOpenGL

Singleton factory for all attributes to enable easy state-sorting
  Including shaders
