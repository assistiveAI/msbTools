#include "bones.h"
#include "ofGraphics.h"
#include "guiUtils.h"
#include "vmath.h"

bones::bones()
{
    time    = 0;
    id      = -2;

    leftElbowRot.identity();
    leftShoulderRot.identity();

    rightElbowRot.identity();
    rightShoulderRot.identity();

}

bones::~bones()
{
    //dtor
}

bones::bones(const bones & B)
{
    reset();
    id = B.id;
    time = B.time;

    for( int i = 0; i < B.skeleton.size(); i++)
        skeleton.push_back( B.skeleton[i] );


    leftElbowRot     = B.leftElbowRot;
    leftShoulderRot  = B.leftShoulderRot;

    rightElbowRot     = B.rightElbowRot;
    rightShoulderRot  = B.rightShoulderRot;

}

void bones::init(int id_, bones & sister)
{
    id = id_;

    for( int i = 0; i < sister.skeleton.size(); i++)
        skeleton.push_back( sister.skeleton[i] );



}

void bones::init(int id_)
{
    reset();

    id = id_;

    // head
    addBoneToSkeleton( 190, 50, 600, 0, "head_t" );
    addBoneToSkeleton( 190, 90, 600, 0, "head_b" );

    // neck
    addBoneToSkeleton( 190, 120, 600, connections["head_b"], "neck" );

    // rigth arm
    addBoneToSkeleton( 210, 120, 600, connections["neck"], "r_s" );
    addBoneToSkeleton( 260, 121, 600, connections["r_s"], "r_e" );
    addBoneToSkeleton( 300, 120, 600, connections["r_e"], "r_h" );

    // left arm
    addBoneToSkeleton( 170, 120, 600, connections["neck"], "l_s" );
    addBoneToSkeleton( 120, 121, 600, connections["l_s"], "l_e" );
    addBoneToSkeleton( 80, 120, 600, connections["l_e"], "l_h" );

    // torso
    addBoneToSkeleton( 190, 160, 600, connections["neck"], "torso" );

    // pelvis
    int mom = connections["torso"];
    addBoneToSkeleton( 190, 210, 600, mom, "pelvis" );

    // right leg
    mom = connections["pelvis"];
    addBoneToSkeleton( 215, 210, 600, mom, "r_hip" );

    mom = connections["r_hip"];
    addBoneToSkeleton( 215, 300, 600, mom, "r_knee" );

    mom = connections["r_knee"];
    addBoneToSkeleton( 215, 380, 600, mom, "r_foot" );

    // left leg
    mom = connections["pelvis"];
    addBoneToSkeleton( 165, 210, 600, mom, "l_hip" );

    mom = connections["l_hip"];
    addBoneToSkeleton( 165, 300, 600, mom, "l_knee" );

    mom = connections["l_knee"];
    addBoneToSkeleton( 165, 380, 600, mom, "l_foot" );


    calculateBoneAngles();

}


void bones::addBoneToSkeleton( int x, int y, int z, int parent, string name )
{
    bone tempBone;
    skeleton.push_back(tempBone);

    int i = skeleton.size() -1 ;
    skeleton[i].loc.set(x,y,z);
    skeleton[i].name   = name;
    skeleton[i].parent = parent;

    connections.insert(pair<string, int>(name, i));

    findChildren(skeleton[i].parent,i);
}

void bones::addBoneSansParent( int x, int y, int z, string name )
{
    bone tempBone;
    skeleton.push_back(tempBone);

    int i = skeleton.size() -1 ;
    skeleton[i].loc.set(x,y,z);
    skeleton[i].name   = name;
    connections.insert(pair<string, int>(name, i));


}

void bones::findParent(int me, string name)
{
    for( int i = 0; i < skeleton.size(); i++)
    {
        if( skeleton[i].name == name ){
             skeleton[me].parent = i;
        }

    }
}


void bones::reset()
{

    skeleton.clear();

    selectedSide    = 0;
    selectedJoint   = 0;

}

void bones::draw()
{

    ofSetColor(255,255,255);
    for( int i = 0; i < skeleton.size(); i++)
    {

        ofFill();
        colorJoint(skeleton[i].name);
        ofRect( skeleton[i].loc.x-5, skeleton[i].loc.y-5, 10,10);
        ofRect( skeleton[i].loc.z-5, skeleton[i].loc.y-5, 10,10);
        ofSetColor(255,0,0);
        ofRect( skeleton[i].loc.x, skeleton[i].loc.y, 1,1);
        ofRect( skeleton[i].loc.z, skeleton[i].loc.y, 1,1);
        ofNoFill();
        ofRect( skeleton[i].loc.x-5, skeleton[i].loc.y-5, 10,10);
        ofRect( skeleton[i].loc.z-5, skeleton[i].loc.y-5, 10,10);

        if( skeleton[i].parent != i )
        {
            int p = skeleton[i].parent;
            ofLine( skeleton[i].loc.x, skeleton[i].loc.y,skeleton[p].loc.x, skeleton[p].loc.y);
            ofLine( skeleton[i].loc.z, skeleton[i].loc.y,skeleton[p].loc.z, skeleton[p].loc.y);
        }

    }

    /*for( int i = 0; i < skeleton.size(); i++)
    {
        char* myChar=new char[4];
        sprintf(myChar,"%i",i);
        string myName =skeleton[i].name+myChar;
         ofDrawBitmapString(myName,skeleton[i].loc.x-5, skeleton[i].loc.y-8);
         ofDrawBitmapString(myName,skeleton[i].loc.z-5, skeleton[i].loc.y-8);
    }*/
   if(selectedJoint >= 0 )
   {
    ofDrawBitmapString(skeleton[selectedJoint].name,skeleton[selectedJoint].loc.x+5, skeleton[selectedJoint].loc.y-8);
    ofDrawBitmapString(skeleton[selectedJoint].name,skeleton[selectedJoint].loc.z+5, skeleton[selectedJoint].loc.y-8);
   }

}

void bones::colorJoint( string name )
{
    //map<string,int>::iterator it = connections.find(name);
    //int myPart = it->second;
    if( name == "r_knee" || name == "r_hip" || name == "r_foot" )
        ofSetColor(180,180,180);
    else if( name == "r_s" || name == "r_h" || name == "r_e" )
        ofSetColor(200,200,200);
    else
        ofSetColor(255,255,255);

}

bool bones::hitTest(int x, int y)
{
    for( int i = 0; i < skeleton.size(); i++)
    {
        ofRectangle hitArea = ofRectangle(skeleton[i].loc.x-5, skeleton[i].loc.y-5,10,10);
        if( pointInRectangle(hitArea,x,y) )
        {
            selectedSide = 0;
            selectedJoint = i;
            return true;
        }

        hitArea = ofRectangle(skeleton[i].loc.z-5, skeleton[i].loc.y-5,10,10);
        if( pointInRectangle(hitArea,x,y) )
        {
            selectedSide    = 1;
            selectedJoint   = i;
            return true;
        }

    }

    return false;
}

void bones::moveSelectedTo( int x, int y, int part, bool bMoveChildren )
{
    selectedJoint = part;
    moveSelectedTo( x, y, bMoveChildren );
}

void bones::moveSelectedTo( int x, int y, bool bMoveChildren )
{
    int xChange = x-skeleton[ selectedJoint ].loc.x;
    int yChange = y-skeleton[ selectedJoint ].loc.y;
    int zChange = x-skeleton[ selectedJoint ].loc.z;


    if( selectedSide == 0) skeleton[ selectedJoint ].loc.set(x,y,skeleton[selectedJoint].loc.z);
    else   skeleton[ selectedJoint ].loc.set(skeleton[selectedJoint].loc.x,y,x);

   if(bMoveChildren)
   {
        for( int i = 0; i < skeleton[ selectedJoint ].children.size(); i++)
        {
            int j = skeleton[selectedJoint].children[i];

            if( selectedSide == 0) skeleton[ j ].loc.x+=xChange;
            else skeleton[ j ].loc.z+=zChange;

            skeleton[ j ].loc.y+=yChange;

        }
   }

}

void bones::calculateBoneAngles()
{


    //need to multiply with reverse
    //spine
    skeleton[2].dist = ( skeleton[2].loc - skeleton[skeleton[2].parent].loc);
    skeleton[2].dist.normalize();
    skeleton[9].dist = ( skeleton[9].loc - skeleton[skeleton[9].parent].loc);
    skeleton[9].dist.normalize();
    skeleton[10].dist = ( skeleton[10].loc - skeleton[skeleton[10].parent].loc);
    skeleton[10].dist.normalize();
    calculateSpineAxis(neckRot,torsoRot,pelvisRot);

    //right Arm
    //see if hand is under elbow, and reverse if it is!
    skeleton[4].dist = ( skeleton[4].loc - skeleton[skeleton[4].parent].loc);
    skeleton[4].dist.normalize();
    skeleton[5].dist = ( skeleton[5].loc - skeleton[skeleton[5].parent].loc);
    skeleton[5].dist.normalize();
    calculateAxis(4,5,rightShoulderRot,rightElbowRot, -1);

    //left Arm
    //see if hand is under elbow, and reverse if it is!
    skeleton[7].dist = (skeleton[skeleton[7].parent].loc - skeleton[7].loc);
    skeleton[7].dist.normalize();
    skeleton[8].dist = (skeleton[skeleton[8].parent].loc - skeleton[8].loc);
    skeleton[8].dist.normalize();
    calculateAxis(7,8,leftShoulderRot,leftElbowRot, 1);

    //adjust to neck
    Matrix4f tempShoulder, tempNeck, tempElbow;

    tempShoulder.identity();
    tempElbow.identity();
    tempNeck.identity();
    //tempNeck.createTranslation(1,1,1);

    tempShoulder.setRotation(leftShoulderRot);
    tempElbow.setRotation(leftElbowRot);

    //tempShoulder.createTranslation(1,1,1);
    //tempElbow.createTranslation(1,1,1);


    tempNeck.setRotation(neckRot);
    tempNeck=tempNeck.inverse();

    tempShoulder = tempShoulder * tempNeck;
    tempElbow = tempElbow * tempNeck;


    leftShoulderRot.data[0]=tempShoulder.data[0];
    leftShoulderRot.data[1]=tempShoulder.data[1];
    leftShoulderRot.data[2]=tempShoulder.data[2];

    leftShoulderRot.data[3]=tempShoulder.data[4];
    leftShoulderRot.data[4]=tempShoulder.data[5];
    leftShoulderRot.data[5]=tempShoulder.data[6];

    leftShoulderRot.data[6]=tempShoulder.data[8];
    leftShoulderRot.data[7]=tempShoulder.data[9];
    leftShoulderRot.data[8]=tempShoulder.data[10];

    //right Shoulder
    tempShoulder.setRotation(rightShoulderRot);
    tempElbow.setRotation(rightElbowRot);

    tempNeck.setRotation(neckRot);
    tempNeck=tempNeck.inverse();

    tempShoulder = tempShoulder * tempNeck;
    tempElbow = tempElbow * tempNeck;


    rightShoulderRot.data[0]=tempShoulder.data[0];
    rightShoulderRot.data[1]=tempShoulder.data[1];
    rightShoulderRot.data[2]=tempShoulder.data[2];

    rightShoulderRot.data[3]=tempShoulder.data[4];
    rightShoulderRot.data[4]=tempShoulder.data[5];
    rightShoulderRot.data[5]=tempShoulder.data[6];

    rightShoulderRot.data[6]=tempShoulder.data[8];
    rightShoulderRot.data[7]=tempShoulder.data[9];
    rightShoulderRot.data[8]=tempShoulder.data[10];

    //right leg
    skeleton[12].dist = (skeleton[12].loc - skeleton[skeleton[12].parent].loc );
    skeleton[12].dist.normalize();
    skeleton[13].dist = (skeleton[13].loc - skeleton[skeleton[13].parent].loc);
    skeleton[13].dist.normalize();
    calculateLegAxis(12,13,rightHipRot,rightKneeRot, -1);

    //left leg
    //quick fix for OF problem
    skeleton[15].dist = (skeleton[skeleton[15].parent].loc - skeleton[15].loc);
    skeleton[16].dist = (skeleton[skeleton[16].parent].loc - skeleton[16].loc);
    skeleton[15].dist.normalize();
    skeleton[16].dist.normalize();

    calculateLegAxis(15,16,leftHipRot,leftKneeRot, 1);
}


void bones::calculateSpineAxis(Matrix3f & rotA, Matrix3f & rotB, Matrix3f & rotC){

    // Axis of neck:
    /*


    neck is 2
    torso is 9
    pelvis is 10

    first calculate torso
    which is pelvis to torso

    then calculate neck
    which is torso to neck
    taking into account the torso rotation

    yAxis = "head" to neck - need another neck!

    xAxis = neck to left Shoulder

    zAxis = y cross x

    */

    Vector3f xVec, yVec, zVec;


    //calculate pelvis
    //xVec is tricky here, but we can set zVec to  0,0,1 to get our xVec
    zVec = Vector3f(0,0,1);

    yVec.x = (skeleton[9].loc - skeleton[10].loc).x ;  // yAxis positive  - because of OF 0,0 starting lower left
    yVec.y = -(skeleton[9].loc - skeleton[10].loc).y ;  // yAxis positive  - because of OF 0,0 starting lower left
    yVec.z = (skeleton[9].loc - skeleton[10].loc).z ;  // yAxis positive  - because of OF 0,0 starting lower left

    yVec.normalize();

    xVec = yVec.crossProduct(zVec);
    xVec.normalize();

    zVec = xVec.crossProduct(yVec);
    zVec.normalize();

    rotC.data[0]=xVec.x;
    rotC.data[1]=xVec.y;
    rotC.data[2]=xVec.z;

    rotC.data[3]=yVec.x;
    rotC.data[4]=yVec.y;
    rotC.data[5]=yVec.z;

    rotC.data[6]=zVec.x;
    rotC.data[7]=zVec.y;
    rotC.data[8]=zVec.z;


    zVec = Vector3f(0,0,1);

    yVec.x = (skeleton[2].loc - skeleton[9].loc).x ;  // yAxis positive  - because of OF 0,0 starting lower left
    yVec.y = -(skeleton[2].loc - skeleton[9].loc).y ;  // yAxis positive  - because of OF 0,0 starting lower left
    yVec.z = (skeleton[2].loc - skeleton[9].loc).z ;  // yAxis positive  - because of OF 0,0 starting lower left

    yVec.normalize();

    xVec = yVec.crossProduct(zVec);
    xVec.normalize();

    zVec = xVec.crossProduct(yVec);
    zVec.normalize();

    rotB.data[0]=xVec.x;
    rotB.data[1]=xVec.y;
    rotB.data[2]=xVec.z;

    rotB.data[3]=yVec.x;
    rotB.data[4]=yVec.y;
    rotB.data[5]=yVec.z;

    rotB.data[6]=zVec.x;
    rotB.data[7]=zVec.y;
    rotB.data[8]=zVec.z;

    //incorporate pelvis into torso
    Matrix4f tempPelvis, tempTorso;

    tempPelvis.setRotation(rotC);
    tempTorso.setRotation(rotB);

    tempTorso= tempTorso * tempPelvis.inverse();

    rotB.data[0]=tempTorso.data[0];
    rotB.data[1]=tempTorso.data[1];
    rotB.data[2]=tempTorso.data[2];

    rotB.data[3]=tempTorso.data[4];
    rotB.data[4]=tempTorso.data[5];
    rotB.data[5]=tempTorso.data[6];

    rotB.data[6]=tempTorso.data[8];
    rotB.data[7]=tempTorso.data[9];
    rotB.data[8]=tempTorso.data[10];

    //shoulder Point - called neck, but is misleading!
    zVec = Vector3f(0,0,-1);

    xVec.x = (skeleton[3].loc - skeleton[6].loc).x ;  //so xAxis is negative
    xVec.y = -(skeleton[3].loc - skeleton[6].loc).y ;  //so xAxis is negative and oh, OF woes...
    xVec.z = (skeleton[3].loc - skeleton[6].loc).z ;  //so xAxis is negative

    xVec.normalize();

    yVec = xVec.crossProduct(zVec);
    yVec.normalize();

    zVec= xVec.crossProduct(yVec);

    rotA.data[0]=xVec.x;
    rotA.data[1]=xVec.y;
    rotA.data[2]=xVec.z;

    rotA.data[3]=yVec.x;
    rotA.data[4]=yVec.y;
    rotA.data[5]=yVec.z;

    rotA.data[6]=zVec.x;
    rotA.data[7]=zVec.y;
    rotA.data[8]=zVec.z;


    Matrix4f tempNeck;

    tempNeck.setRotation(rotA);
    //tempTorso.setRotation(rotB);
    tempNeck= tempNeck * tempTorso.inverse();
    tempNeck= tempNeck * tempPelvis.inverse();
    rotA.data[0]=tempNeck.data[0];
    rotA.data[1]=tempNeck.data[1];
    rotA.data[2]=tempNeck.data[2];

    rotA.data[3]=tempNeck.data[4];
    rotA.data[4]=tempNeck.data[5];
    rotA.data[5]=tempNeck.data[6];

    rotA.data[6]=tempNeck.data[8];
    rotA.data[7]=tempNeck.data[9];
    rotA.data[8]=tempNeck.data[10];
}

void bones::calculateAxis(int me, int mom, Matrix3f & rotA, Matrix3f & rotB, int flip){


    Vector3f xVec1 = Vector3f( skeleton[me].dist.x,-skeleton[me].dist.y, skeleton[me].dist.z );
    Vector3f xVec2 = Vector3f( skeleton[mom].dist.x,-skeleton[mom].dist.y, skeleton[mom].dist.z );



    Vector3f zVec = xVec2.crossProduct( xVec1 );
    zVec.normalize();

    Vector3f yVec= xVec1.crossProduct (zVec);
    yVec.normalize();


    rotA.data[0]= xVec1.x;
    rotA.data[1]= xVec1.y;
    rotA.data[2]= xVec1.z;

    rotA.data[3]= -flip * yVec.x;
    rotA.data[4]= -flip * yVec.y;
    rotA.data[5]= -flip * yVec.z;

    rotA.data[6]=flip * zVec.x;
    rotA.data[7]=flip * zVec.y;
    rotA.data[8]=flip * zVec.z;

    //if (xVec1.y>0)

    float zRot = xVec1.dotProduct(xVec2);
    zRot = acos(zRot);

    rotB = Matrix3f::createRotationAroundAxis(0,0, flip * zRot);

}

void bones::calculateLegAxis(int me, int mom, Matrix3f & rotA, Matrix3f & rotB, int flip){

    Vector3f yVec1 = Vector3f( skeleton[me].dist.x,-skeleton[me].dist.y, skeleton[me].dist.z );
    Vector3f yVec2 = Vector3f( skeleton[mom].dist.x,-skeleton[mom].dist.y, skeleton[mom].dist.z );

    Vector3f xVec = yVec1.crossProduct( yVec2 );
    xVec.normalize();

    Vector3f zVec= xVec.crossProduct (yVec1);
    zVec.normalize();


    //is column major, so we need to transpose!
    rotA.data[0]= xVec.x;
    rotA.data[1]= xVec.y;
    rotA.data[2]= xVec.z;

    rotA.data[3]=flip * yVec1.x;
    rotA.data[4]=flip * yVec1.y;
    rotA.data[5]=flip * yVec1.z;

    rotA.data[6]=flip * zVec.x;
    rotA.data[7]=flip * zVec.y;
    rotA.data[8]=flip * zVec.z;

    //if (xVec1.y>0)

    float xRot = yVec2.dotProduct(yVec1);
    xRot = acos(xRot);

    rotB = Matrix3f::createRotationAroundAxis(-xRot,0,0);

}

void bones::calculate3dRotation( int me, int mom, Vector3f xAxis, Vector3f yAxis, ofxVec3f & rotA, ofxVec3f & rotB )
{

    Vector3f xVec1 = Vector3f( skeleton[me].dist.x,skeleton[me].dist.y, skeleton[me].dist.z );
    Vector3f xVec2 = Vector3f( skeleton[mom].dist.x,skeleton[mom].dist.y, skeleton[mom].dist.z );

    Vector3f zVec = xVec1.crossProduct( xVec2 );
    zVec.normalize();

    Vector3f xOnAPlane = zVec.crossProduct( yAxis );
    xOnAPlane.normalize();

    float zAngle = xVec1.dotProduct( xOnAPlane );

    Vector3f zOnAPlane = xOnAPlane.crossProduct( yAxis );
    zOnAPlane.normalize();
    float xAngle = zVec.dotProduct(zOnAPlane);

    float yAngle = xOnAPlane.dotProduct( xAxis);

    rotA.x = 180-(RAD_TO_DEG*acos(xAngle));
    rotA.y = -(180-(RAD_TO_DEG*acos(yAngle)));
    rotA.z = -(180-(RAD_TO_DEG*acos(zAngle)));

/*
    //for raised arm
    if (xVec1.y>0)
        rotA.z = -(RAD_TO_DEG*acos(zAngle));


    //for right arm
    if (xVec1.x<0){
        rotA.x =180-(RAD_TO_DEG*acos(xAngle));
        rotA.z =180-rotA.z;
    }

    //for arm forward
    //if (xVec1.z<0)
    //    rotA.x=-rotA.x;
*/
    cout << rotA.x << " " << rotA.y << " " << rotA.z << endl;

    // hand to elbow vector
    float handAngle =  xVec2.dotProduct( xVec1 );
    rotB.x = 0;
    rotB.y = 0;

    if (xVec1.x<0)
        rotB.z = -(RAD_TO_DEG*acos(handAngle));
    else
        rotB.z = (RAD_TO_DEG*acos(handAngle));
}

void bones::calculateRight3dRotation( int me, int mom, Vector3f xAxis, Vector3f yAxis, ofxVec3f & rotA, ofxVec3f & rotB )
{

    Vector3f xVec1 = Vector3f( skeleton[me].dist.x,skeleton[me].dist.y, skeleton[me].dist.z );
    Vector3f xVec2 = Vector3f( skeleton[mom].dist.x,skeleton[mom].dist.y, skeleton[mom].dist.z );

    Vector3f yVec = xVec1.crossProduct( xVec2 );
    yVec.normalize();

    Vector3f zVec =xVec1.crossProduct( yVec );

    //first rotate around z so that x-vector is on the absolute XZ plane
    //find representation of x-Vector on XY Plane
    Vector3f xOnAPlane = zVec.crossProduct( yAxis  );
    xOnAPlane.normalize();
    float zAngle = xVec1.dotProduct( xOnAPlane );

    //now rotate around x so that zVec is in YZ Plane
    //find representation of zVec on XZ plane
    Vector3f zOnAPlane = xOnAPlane.crossProduct( yAxis);
    zOnAPlane.normalize();
    float xAngle = zVec.dotProduct(zOnAPlane);

    //finally rotate back to beginning
    float yAngle = xOnAPlane.dotProduct( xAxis );



    rotA.x = (RAD_TO_DEG*acos(xAngle));
    rotA.y = (RAD_TO_DEG*acos(yAngle));
    rotA.z = (RAD_TO_DEG*acos(zAngle));

    // hand to elbow vector
    float handAngle =  xVec2.dotProduct( xVec1 );
    rotB.x = 0;
    rotB.y = (RAD_TO_DEG*acos(handAngle));
    rotB.z = 0;

}


bool bones::findChildren( int me, int child )
{
    bool bHasChild = true;
    for( int i = 0; i < skeleton[me].children.size(); i++)
    {
        if( skeleton[me].children[i] == child ) bHasChild = false;
    }

    if(bHasChild && child != me) skeleton[me].children.push_back(child);
    if( skeleton[me].parent != me ) findChildren( skeleton[me].parent, child );


    return false;

}

