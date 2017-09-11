###
# 
# Plugin for blender to load graphs
#
###

import bpy
import json
from math import acos
from mathutils import Vector


## Operator that does all the work

class GraphState(bpy.types.Operator):
    """Script to create GraphState lattice"""      # blender will use this as a tooltip for menu items and buttons.
    bl_idname = "object.graphstate"        # unique identifier for buttons and menu items to reference.
    bl_label = "generate GraphState"         # display name in the interface.
    bl_options = {'REGISTER', 'UNDO'}  # enable undo for the operator.

    filepath = bpy.props.StringProperty(subtype="FILE_PATH")

########### plugin related stuff for the interface

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return{"RUNNING_MODAL"}

    def draw(self,context):
        layout = self.layout
        col = layout.column()
        col.label(text="Choose the json data to be visualized!")


########### Code that actually does something
    def execute(self,context):
        """
        Generates the lattice from a file
        using more efficient implementation than operators
        """

        bpy.ops.object.select_all(action='DESELECT')

        #create sphere stencil
        bpy.ops.mesh.primitive_uv_sphere_add(segments = 3, ring_count=2, size=0.2,location = [-1,-1,0])
        bpy.ops.object.shade_smooth()
        sphere = bpy.context.object

        #maxlayers = 8
        #cur_layer = 0

        #read json
        with open(self.filepath) as json_data:
            json_data.seek(10)
            data = json_data.read()
            lattice = json.loads(data)

        # Draw nodes
        for node in lattice["nodes"]:
            ob = sphere.copy()
            ob.location = lattice["nodes"][node]
            bpy.context.scene.objects.link(ob)
            # only make a fraction visible at the same time
            #cur_layer = (cur_layer+1)%maxlayers
            #ob.layers = [ i==cur_layer for i in range(len(ob.layers)) ]

        bpy.ops.object.select_all(action='DESELECT')

        #create cylinder stencil
        bpy.ops.mesh.primitive_cylinder_add(vertices=8, radius=0.05, depth=1, location=[-1,-1,0])
        bpy.ops.object.shade_smooth()
        cylinder = bpy.context.object

        # Draw bonds
        for edge in lattice["edges"]:

            # Extracting locations
            first_loc = lattice["nodes"][str(edge[0])]
            second_loc = lattice["nodes"][str(edge[1])]

            # Useful values
            diff = tuple([c2-c1 for c2, c1 in zip(first_loc, second_loc)])
            center = tuple([(c2+c1)/2 for c2, c1 in zip(first_loc, second_loc)])
            magnitude = pow(sum([(c2-c1)**2
                            for c1, c2 in zip(first_loc, second_loc)]), 0.5)

            # Euler rotation calculation, (Vector from mathutils, acos from math)
            Vaxis = Vector(diff).normalized()
            Vobj = Vector((0,0,1))
            Vrot = Vobj.cross(Vaxis)
            angle = acos(Vobj.dot(Vaxis))

            ob = cylinder.copy()
            ob.location = center
            ob.rotation_mode = "AXIS_ANGLE"
            ob.scale[2] = magnitude
            ob.rotation_axis_angle[0] = angle
            ob.rotation_axis_angle[1:] = Vrot
            bpy.context.scene.objects.link(ob)
        
        bpy.ops.object.select_all(action="DESELECT")    
        bpy.ops.object.select_pattern(pattern="Cylinder")
        bpy.ops.object.select_pattern(pattern="Sphere")
        bpy.ops.object.delete()

        bpy.context.scene.update()
        return {'FINISHED'}


def register():
    try:
        bpy.utils.unregister_class(GraphState)
    except RuntimeError:
        pass
    bpy.utils.register_class(GraphState)


def unregister():
    bpy.utils.unregister_class(GraphState)

# This allows you to run the script directly from blenders text editor
# to test the addon without having to install it.
if __name__ == "__main__":
    register()
