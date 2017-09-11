// IE9
if(typeof console === "undefined") { var console = { log: function (logMsg) { } }; }

var camera, controls, scene, renderer, material;
var spheregeometry;
var vertices=[];

/*objects in the 3d scene*/
var primalDefects = new THREE.Geometry();
var dualDefects = new THREE.Geometry();
var notWorkingDefect = new THREE.Geometry();
var debugDefects1 = new THREE.Geometry();
var debugDefects2 = new THREE.Geometry();

var linePrimalBuffer = new THREE.BufferGeometry();
var lineDualBuffer = new THREE.BufferGeometry();
var lineNotWorkingBuffer = new THREE.BufferGeometry();
var lineDebug1Buffer = new THREE.BufferGeometry();
var lineDebug2Buffer = new THREE.BufferGeometry();

/*Materials*/
var materialPrimal = new THREE.LineBasicMaterial({ color: 0xFF0000, linewidth: 2, transparent: true, opacity: 1});
var materialDual = new THREE.LineBasicMaterial({ color: 0x0000FF, linewidth: 2, transparent: true, opacity: 1 });
var materialInjection = new THREE.MeshLambertMaterial({color: "green"});
var materialNotWorking = new THREE.LineBasicMaterial({ color: 0xFFFF00, linewidth: 20, transparent: false, opacity: 1 });
var materialDebug1 = new THREE.LineBasicMaterial({ color: 0x00FF00, linewidth: 5, transparent: false, opacity: 1 });
var materialDebug2 = new THREE.LineBasicMaterial({ color: 0xf442eb, linewidth: 5, transparent: false, opacity: 1 });

/*bounding box*/
var bBox = [Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER,
             Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER];//dimension 6: 3 mins, 3 maxes

/*statistics*/
var stats = new Stats();
var objectsToClick = [];

/*standby render*/
var renderLastMove = Date.now();
var renderRunning = false;
var renderStandbyAfter = 2000; // ms

// Run on startup
$(document).ready(main);
window.addEventListener("mousedown", function(){/*console.log("start"); */renderRunning = false; startAnimation()}, true);
window.addEventListener("mouseup", function(){/*console.log("stop"); */renderRunning = false}, true);
window.addEventListener("wheel", function(){/*console.log("scroll");*/renderRunning = false; startAnimation()}, true);

window.onunload = function()
{
  scene = null;
}

// ************ STARTUP *****************
function main()
{
  stats.showPanel( 1 ); // 0: fps, 1: ms, 2: mb, 3+: custom
  document.body.appendChild( stats.dom );

  init();
  animate();
  //startAnimation();
}

function de2ra(degree)  { return degree*(Math.PI/180); }

function add_vertex(x,y,z, material) {

    var sphere = new THREE.Mesh( spheregeometry, material);
    sphere.position.x=x;
    sphere.position.y=y;
    sphere.position.z=z;
    scene.add(sphere);
    vertices.push(sphere);
}

function add_edge2(x1, y1, z1, x2, y2, z2, lineSegmentsGeometry)
{
    lineSegmentsGeometry.vertices.push(new THREE.Vector3(x1, y1, z1));
    lineSegmentsGeometry.vertices.push(new THREE.Vector3(x2, y2, z2));
    
    updateBoundingBox(x1, y1, z1);
    updateBoundingBox(x2, y2, z2);
}

function add_edge2Index(index, graphobject, lineSegmentsGeometry)
{
    var a = graphobject.nodes[graphobject.edges[index][0]];
    var b = graphobject.nodes[graphobject.edges[index][1]];
    
    add_edge2(a[0], a[1], a[2], b[0], b[1], b[2], lineSegmentsGeometry);
}

function makePins(boxparam, bigcube)
{
	var geometry = new THREE.BoxGeometry(1, 1, 1);

	for(var i=0; i<2; i++)
	{
		var cube = new THREE.Mesh( geometry, new THREE.MeshBasicMaterial({
		    color: 'red'
		  }) );

		cube.position.x = 0.5/*coordonata nu colt ci centrul*/ 
				+ boxparam[2] + boxes.types[boxparam[1]][0] - (1-i)*2*2/*dist*/- 1/*acuma e cu o unitate prea in dreapta*/;
		cube.position.y = 0.5 + boxparam[3] + boxes.types[boxparam[1]][1] - 1/*acuma e cu o unitate prea in sus*/;
		cube.position.z = 0.5 + boxparam[4] + boxes.types[boxparam[1]][2];

		scene.add(cube);
	}
	
}

function makeBoundingBox()
{
  var width = bBox[3] - bBox[0];
  var height = bBox[4] - bBox[1];
  var depth = bBox[5] - bBox[2];
  
  console.log(width + " " + height + " " + depth);
  for(var i=0; i<6; i++)
    console.log(bBox[i]);
    
  console.log(Math.ceil(width/6) + " " + Math.ceil(height/6) + " " + Math.ceil(depth/6));
  
  var geometry = new THREE.BoxGeometry(width, height, depth);
		
	geometry.translate(bBox[0] + width/2, 
	                bBox[1] + height/2, 
	                bBox[2] + depth/2);
                    
  var cube = new THREE.Mesh( geometry, 
   new THREE.MeshBasicMaterial({wireframe: true, color: 'black'})
   );
	cube.updateMatrix();
	
	scene.add(cube);
}

function updateBoundingBox(cw, ch, cd)
{
  if(cw < bBox[0])
    bBox[0] = cw;
  else if (cw > bBox[3])
    bBox[3] = cw;
    
  if(ch < bBox[1])
    bBox[1] = ch;
  else if (ch > bBox[4])
    bBox[4] = ch;
    
  if(cd < bBox[2])
    bBox[2] = cd;
  else if (cd > bBox[5])
    bBox[5] = cd;
}

function makeBoxes(graph)
{
  var boxes = new THREE.Geometry();
	
	for (var i=0; i<Object.keys(graph.nodes).length; ++i)
  {
    var key = Object.keys(graph.nodes)[i];
    var boxparam = graph.nodes[key];
    
		var width = 0.5;
		var height = 0.5;
		var depth = 0.5;
		
		var geometry = new THREE.BoxGeometry(width, height, depth);
		
		geometry.translate(boxparam[0] + width/2,
		                    boxparam[1] + height/2,
		                    boxparam[2] + depth/2);
		                    
		//for box start coordinates
		updateBoundingBox(boxparam[0], boxparam[1], boxparam[2]);
		//include box dimensions, too
		updateBoundingBox(boxparam[0] + width, boxparam[1] + height, boxparam[2] + depth);
		
		var cube = new THREE.Mesh( geometry);
		cube.updateMatrix();
		
		boxes.merge(cube.geometry, cube.matrix);
		cube = null;
		
		/*debugMessage(i+"b", boxparam[2] + geometry.parameters.width/2,
		                    boxparam[3] + geometry.parameters.height/2,
		                    boxparam[4] + geometry.parameters.depth/2);*/
	}
	
	var cubes = new THREE.Mesh( boxes, 
	
	    //new THREE.MeshBasicMaterial({wireframe: true, color: 'blue'})
      
    new THREE.MeshLambertMaterial({color: 'blue', side: 2, shading: THREE.FlatShading})
  );
	
	scene.add(cubes);
}

function debugMessage(message, x, y, z)
{
  var s1 = makeTextSprite2(message);

  s1.position.x = x;
  s1.position.y = y;
  s1.position.z = z;
  scene.add(s1);
}

function findPoint(point, graphobject, objToFill)
{
  //make edges
  var found = false;
  var i = -1;
  for (i = 0; i < graphobject.edges.length && !found; i++)
  {
      var a = graphobject.nodes[graphobject.edges[i][0] - 1];
      var b = graphobject.nodes[graphobject.edges[i][1] - 1];
      
      var eq = 0;
      var included = true;
      for(var j=0; j<3; j++)
      {
        var compj = point.getComponent(j);
        if(a[j+1] == compj && b[j+1] == compj)
        {
          eq++;
        }
        
        if (a[j+1] < b[j+1])
        {
          included = (a[j+1] <= compj) && (compj <= b[j+1])
        }
        else if(a[j+1] > b[j+1])
        {
          included = (b[j+1] <= compj) && (compj <= a[j+1])
        }
      }
              
      if(eq == 2 && included)
      {
        found = true;
        console.log("found");
        //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], notWorkingDefect);
      }
  }
  
  if(found)
  {
    var idx1 = graphobject.edges[i][0];
    var idx2 = graphobject.edges[i][1];
    
    add_edge2Index(i, graphobject, notWorkingDefect);
    
    for (var j = i+1; j < graphobject.edges.length; j++)
    {
      if(graphobject.edges[j][0] == idx1)
      {
        idx1 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx1)
      {
        idx1 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][0] == idx2)
      {
        idx2 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx2)
      {
        idx2 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
    }
    
    for (var j = i-1; j >= 0; j--)
    {
      if(graphobject.edges[j][0] == idx1)
      {
        idx1 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx1)
      {
        idx1 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][0] == idx2)
      {
        idx2 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx2)
      {
        idx2 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
    }
    
  }
  
  return found;
}

function makeGeometry(graphobject, objToFill)
{

		//make edges
    for (var i = 0; i < graphobject.edges.length; i++)
    {
        var a = graphobject.nodes[graphobject.edges[i][0] - 1];
        var b = graphobject.nodes[graphobject.edges[i][1] - 1];

				if(objToFill != null)
				{
				  //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], objToFill);
				  add_edge2Index(i, graphobject, objToFill);
				}
				else
				{
					//if(otherside >= 2)//asta e pentru qubits si nu pentru celule. eu calculez in celule. in pula mea iar am incurcat...
				  otherside = 0;
				  for(var j=1; j<4; j++)
					  if(a[j]%2 != 0)
						  otherside++;
						
				  if(otherside == 3)
				  {//celulele primare au coordonate impare, iar cele duale au coordonate pare
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], primalDefects);
  				  add_edge2Index(i, graphobject, primalDefects);
				  }
				  else
				  {
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], dualDefects);
            add_edge2Index(i, graphobject, dualDefects);
				  }
				  
				  if(i == graphobject.edges.length - 1)
				  {
				    //the connection that was not constructed should be highlighted with a dual segment
				    //to be distinct from the green blocks
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], notWorkingDefect);
				    add_edge2Index(i, graphobject, notWorkingDefect);
				  }
        }
    }
}

function init() {
    // Set up the container and renderer
    var $container = document.getElementById('container');
    var w=window,d=document,e=d.documentElement,g=d.getElementsByTagName('container')[0],x=w.innerWidth||e.clientWidth||g.clientWidth,y=w.innerHeight||e.clientHeight||g.clientHeight;
    var WIDTH=x;
    var HEIGHT=y;


    // Renderer
		renderer = Detector.webgl? new THREE.WebGLRenderer({preserveDrawingBuffer: true}): new THREE.CanvasRenderer();
    //renderer = new THREE.WebGLRenderer( { antialias: true });
    renderer.setSize(WIDTH, HEIGHT-20);
    renderer.setClearColor( 0xffffff, 1 );
    $container.appendChild(renderer.domElement);
    scene = new THREE.Scene();

  // Center the camera
    var sx=0; var sy=0; var sz=0;
    for (var i=0; i<Object.keys(graph.nodes).length; ++i)
    {
        var key = Object.keys(graph.nodes)[i];
        var n = graph.nodes[key];
        if (n[0] > sx){ sx = n[0]; }
        if (n[1] > sy){ sy = n[1]; }
        if (n[2] > sz){ sz = n[2]; }
    }
    console.log("sxyz " + Object.keys(graph.nodes).length + " # " + sx + " " + sy + " " + sz);
    
    // Camera
    camera = new THREE.PerspectiveCamera( 45, WIDTH/HEIGHT, 0.1,10000);
    camera.position.set(sx/2, sy/2, sz/2 + sx*1.5);
    camera.up = new THREE.Vector3(-1,0,0);
    
    scene.add(camera);

    // Models
    spheregeometry = new THREE.SphereGeometry(.5, 3, 3)

    // Lighting
    var ambientLight = new THREE.AmbientLight(0x552222);
    ambientLight.intensity=.1;
    scene.add(ambientLight);

    var pointLight1 = new THREE.PointLight(0xFFFFFF);
    pointLight1.position.x = 1000;
    pointLight1.position.y = 1000;
    pointLight1.position.z = 1300;
    scene.add(pointLight1);
    
    /*var pointLight2 = new THREE.PointLight(0xFFFFFF);
    pointLight2.position.x = -1000;
    pointLight2.position.y = 1000;
    pointLight2.position.z = 1300;
    scene.add(pointLight2);*/

   /* // Materials
    var materialPrimal = new THREE.LineBasicMaterial({ color: 0xFF0000, linewidth: 2, transparent: true, opacity: 1});
    var materialDual = new THREE.LineBasicMaterial({ color: 0x0000FF, linewidth: 2, transparent: true, opacity: 1 });
    var materialInjection = new THREE.MeshLambertMaterial({color: "green"});
    
    var materialDebug1 = new THREE.LineBasicMaterial({ color: 0x00FF00, linewidth: 5, transparent: false, opacity: 1 });
    var materialDebug2 = new THREE.LineBasicMaterial({ color: 0xFFA500, linewidth: 5, transparent: false, opacity: 1 });*/

    // Add controls
    controls = new THREE.TrackballControls(camera, $container);
    controls.target.set( sx/2, sy/2, sz/2 - sx*1.5);
	  controls.rotateSpeed = 2.0;
	  controls.zoomSpeed = 1.2;
	  controls.panSpeed = 0.8;

	  controls.noZoom = false;
	  controls.noPan = false;

	  controls.staticMoving = true;
	  controls.dynamicDampingFactor = 0.3;
	  
		makeBoxes(graph);

  	//makeGeometry(graph, primalDefects);//connects pins to boxes
		//makeGeometry(graph, null);//the circuit
		
		//makeGeometry(graph3, debugDefects1 /*all is in debug*/);
		
		//makeGeometry(graph4, debugDefects2 /*all is in debug*/);
		
    makebuffered(primalDefects, linePrimalBuffer);
    var linePrimal = new THREE.LineSegments(linePrimalBuffer, materialPrimal);
    scene.add(linePrimal);
    primalDefects = null;
    
    //makeBoundingBox();
    
    /*http://stackoverflow.com/questions/32955103/selecting-object-with-mouse-three-js*/
    /*select new defect : mouse down + click*/
    //objectsToClick = [linePrimal, lineDual];
    
    renderer.domElement.addEventListener( 'mousedown', onDocumentMouseDown );
    function onDocumentMouseDown( event ) 
    {
      if (!event.shiftKey)
      {
        return;
      }
      
      event.preventDefault();
      var found = false;
      //dimension of the mouse coordinate window to search
      var dim = 5;
      for(var i=-dim/2; i<dim/2+1 && !found; i++)
      {
        for(var j=-dim/2; j<dim/2+1 && !found; j++)
        {
          found = mouse3dray(event.clientX + i, event.clientY + j);
        }
      }
      
      if(found)
      {
        //linePrimal.material.color.setHex( Math.random() * 0xffffff );
        linePrimal.material.opacity = 0.5;
      }
      else
      {
        linePrimal.material.opacity = 1;
      }
    }
         
}

function mouse3dray(x, y)
{
  //transform screen coordinates into opengl coordinates
  var mouse3D = new THREE.Vector3( ( x / renderer.domElement.innerWidth ) * 2 - 1,   
                            -( y / renderer.domElement.innerHeight ) * 2 + 1,  
                            0.01/*z coordinate is back*/ );    
                             
  var raycaster =  new THREE.Raycaster();                                        
  raycaster.setFromCamera( mouse3D, camera );
  var intersects = raycaster.intersectObjects( objectsToClick );
  
  var found = false;
  if ( intersects.length > 0 ) 
  {
    console.log(intersects[0].point);

    scene.remove(lineNotWorking);
    notWorkingDefect.vertices = [];
    
    found = findPoint(intersects[0].point, graph2, null);
    if(!found)
    {
      found = findPoint(intersects[0].point, graph, null);
    }

    if(found)
    {
      //notWorkingDefect = new THREE.Geometry();
      //lineNotWorkingBuffer = new THREE.BufferGeometry();
      //lineNotWorkingBuffer = new THREE.BufferGeometry();
      makebuffered(notWorkingDefect, lineNotWorkingBuffer);
      var lineNotWorking = new THREE.LineSegments(lineNotWorkingBuffer, materialNotWorking);
      scene.add(lineNotWorking);
      //notWorkingDefect = null;
    }
  }

  return found;
}

function makebuffered(normalgeom, bufferedgeom)
{
  var positions = new Float32Array(normalgeom.vertices.length * 3);
  var indices = new Uint16Array(normalgeom.vertices.length);
  for (var i = 0; i < normalgeom.vertices.length; i++)
  {

      positions[i * 3] = normalgeom.vertices[i].x;
      positions[i * 3 + 1] = normalgeom.vertices[i].y;
      positions[i * 3 + 2] = normalgeom.vertices[i].z;
      indices[i] = i;
  }

  bufferedgeom.addAttribute('position', new THREE.BufferAttribute(positions, 3));
	bufferedgeom.setIndex(new THREE.BufferAttribute(new Uint16Array(indices), 1));
}

function checkTime()
{
  /*either use the standby time 
    or return true when renderRunning is a semaphore*/
  return true;
  //return (renderLastMove + renderStandbyAfter >= Date.now());
}

function startAnimation()
{
  renderLastMove = Date.now();
  
  /*daca nu ruleaza inca animatia*/
  if (!renderRunning)
  {
    //verifica de are voie sa ruleze
    if(checkTime())
    {
      renderRunning = true;
      requestAnimationFrame(animate);
    }
  }
}

function animate()
{
  renderer.render(scene, camera);
      
  if (!checkTime() || !renderRunning)
  {
    renderRunning = false;
  }
  else
  {
    requestAnimationFrame(animate);
  }
  
  controls.update();
  stats.update();
  
  /*console.log(renderRunning);
  if(renderRunning)
  {
    renderer.render(scene, camera);
    requestAnimationFrame(animate);
    controls.update();
    stats.update()
  }*/
}

function makeTextSprite2(message)
{
	var canvas = document.createElement('canvas');
	var size = 1024; // CHANGED
	canvas.width = size;
	canvas.height = size;
	var context = canvas.getContext('2d');
	context.fillStyle = '#000000'; // CHANGED
	context.textAlign = 'center';
	context.font = '200px Arial';
	context.fillText(message, size / 2, size / 2);

	var amap = new THREE.Texture(canvas);
	amap.needsUpdate = true;

	var mat = new THREE.SpriteMaterial({
 		map: amap,
  	transparent: false,
  	useScreenCoordinates: false,
  	color: 0xffffff // CHANGED
	});

	var sp = new THREE.Sprite(mat);
	sp.scale.set( 100, 100, 1 ); // CHANGED

	return sp;
}
