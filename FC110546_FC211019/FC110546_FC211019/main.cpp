#include <iostream>
#include <math.h>
#include <stdlib.h>



using namespace std;

// Global Variables
float angleX = 0.0f, angleY = 0.0f; // Rotation angles
float scale = 1.0f; // Scaling factor

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "stb_image.h"

enum Color { WHITE, BROWN, ORANGE, GREY, BLUE, DARK_GREY, YELLOW, GREEN, DARK_BROWN, DARK_GREEN };

// Translation
float X = 0, Y = 0, Z = 0;
// Rotation
float I = 0, J = 0, K = 0;
// Scale
float SCALE = 1;

// House rotation variables
float houseRotationAngle = 0.0f; // Angle for house rotation
bool houseRotating = false; // Toggle for house rotation
float rotationSpeed = 1.0f; // Speed of rotation

//Textures
GLuint wallTexture, doorTexture, windowTexture, roofTexture, trunkTextureID, leafTextureID;

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);
    }
    else {
        cout << "Failed to load texture: " << filename << endl;
    }

    stbi_image_free(data);
    return textureID;
}

void applyTexture(GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void applyTranslationRotation() {
    glTranslatef(X, Y, Z);
    glRotatef(I, 1, 0, 0);
    glRotatef(J, 0, 1, 0);
    glRotatef(K, 0, 0, 1);
}

void nextColor(Color c) {
    switch (c) {
    case WHITE: glColor3f(0.9, 0.9, 0.9); break;
    case BROWN: glColor3f(0.45, 0.35, 0.25); break;
    case GREY: glColor3f(0.8, 0.8, 0.8); break;
    case DARK_GREY: glColor3f(0.4, 0.4, 0.4); break;
    case BLUE: glColor3f(0.6, 0.75, 0.85); break;
    case ORANGE: glColor3f(0.75, 0.25, 0); break;
    case YELLOW: glColor3f(1, 1, 0); break;
    case GREEN: glColor3f(0, 0.5, 0); break;
    case DARK_GREEN: glColor3f(0.0, 0.3, 0.0); break; // Dark green
    }

}

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Light properties
    GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // No ambient light
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bright diffuse light
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float x, float y, float z, float length, GLuint textureID) {
    GLfloat h = length * 0.5f;

    glEnable(GL_TEXTURE_2D);  // Enable textures
    glBindTexture(GL_TEXTURE_2D, textureID);  // Bind the texture

    glBegin(GL_QUADS);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1, 1); glVertex3f(x + h, y + h, z + h);
    glTexCoord2f(1, 0); glVertex3f(x + h, y - h, z + h);
    glTexCoord2f(0, 0); glVertex3f(x - h, y - h, z + h);
    glTexCoord2f(0, 1); glVertex3f(x - h, y + h, z + h);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1, 1); glVertex3f(x + h, y + h, z - h);
    glTexCoord2f(1, 0); glVertex3f(x + h, y - h, z - h);
    glTexCoord2f(0, 0); glVertex3f(x - h, y - h, z - h);
    glTexCoord2f(0, 1); glVertex3f(x - h, y + h, z - h);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(x - h, y + h, z + h);
    glTexCoord2f(1, 0); glVertex3f(x - h, y - h, z + h);
    glTexCoord2f(0, 0); glVertex3f(x - h, y - h, z - h);
    glTexCoord2f(0, 1); glVertex3f(x - h, y + h, z - h);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(x + h, y + h, z + h);
    glTexCoord2f(1, 0); glVertex3f(x + h, y - h, z + h);
    glTexCoord2f(0, 0); glVertex3f(x + h, y - h, z - h);
    glTexCoord2f(0, 1); glVertex3f(x + h, y + h, z - h);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(x + h, y + h, z + h);
    glTexCoord2f(0, 1); glVertex3f(x - h, y + h, z + h);
    glTexCoord2f(0, 0); glVertex3f(x - h, y + h, z - h);
    glTexCoord2f(1, 0); glVertex3f(x + h, y + h, z - h);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(x + h, y - h, z + h);
    glTexCoord2f(0, 1); glVertex3f(x - h, y - h, z + h);
    glTexCoord2f(0, 0); glVertex3f(x - h, y - h, z - h);
    glTexCoord2f(1, 0); glVertex3f(x + h, y - h, z - h);

    glEnd();

    glDisable(GL_TEXTURE_2D);  // Disable textures after drawing
}


/*void drawWindow(float k, int i, int j) {
    glPushMatrix();
    applyTranslationRotation();
    glRotatef(houseRotationAngle, 0, 1, 0); // Rotate windows with the house

    nextColor(BLUE);
    glTranslatef(0.4 * k * i, 0, j * k / 2 + 0.01 * j);
    glRectf(-0.2 * k, -0.2 * k, 0.2 * k, 0.2 * k);
    glPopMatrix();
}*/

void drawWindow(float k, int i, int j, GLuint textureID) {

    glEnable(GL_TEXTURE_2D);  // Enable textures
    glBindTexture(GL_TEXTURE_2D, textureID);  // Bind the texture

    glPushMatrix();
    applyTranslationRotation();
    glRotatef(houseRotationAngle, 0, 1, 0); // Rotate windows with the house

    nextColor(BLUE);
    glTranslatef(0.4 * k * i, 0, j * k / 2 + 0.01 * j);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.2 * k, -0.2 * k, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.2 * k, -0.2 * k, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.2 * k, 0.2 * k, 0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.2 * k, 0.2 * k, 0);
    glEnd();
    glPopMatrix();
}

void drawFence(float length, float height) {
    float postWidth = 0.05;
    float slatWidth = 0.08;
    float slatSpacing = 0.04;
    float cornerPostWidth = 0.12;
    float groundY = 0;  // Ensures bottom aligns with Y = 0

    float postPositions[4][3] = {
        {-length / 2, groundY, -2.5},
        {length / 2, groundY, -2.5},
        {-length / 2, groundY, 2.5},
        {length / 2, groundY, 2.5}
    };

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(postPositions[i][0], groundY, postPositions[i][2]); // Centered by height/2
        nextColor(DARK_GREY);
        glScalef(cornerPostWidth, height, cornerPostWidth); // Taller posts
        glutSolidCube(1);
        glPopMatrix();
    }

    // Front and back slats
    for (float i = -length / 2 + cornerPostWidth; i < length / 2 - cornerPostWidth; i += (slatWidth + slatSpacing)) {
        glPushMatrix();
        glTranslatef(i, groundY, -2.5);
        nextColor(GREY);
        glScalef(slatWidth, height * 0.9, postWidth);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(i, groundY, 2.5);
        nextColor(GREY);
        glScalef(slatWidth, height * 0.9, postWidth);
        glutSolidCube(1);
        glPopMatrix();
    }

    // Left and right slats
    for (float i = -length / 2 + cornerPostWidth; i < length / 2 - cornerPostWidth; i += (slatWidth + slatSpacing)) {
        glPushMatrix();
        glTranslatef(-2.5, groundY, i);
        nextColor(GREY);
        glScalef(postWidth, height * 0.9, slatWidth);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.5, groundY, i);
        nextColor(GREY);
        glScalef(postWidth, height * 0.9, slatWidth);
        glutSolidCube(1);
        glPopMatrix();
    }

    // Top horizontal rails
    float railY = height * 0.2; // Raised slightly to avoid floating
    glPushMatrix();
    glTranslatef(0, railY, -2.5);
    nextColor(DARK_GREY);
    glScalef(length, 0.04, postWidth);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, railY, 2.5);
    nextColor(DARK_GREY);
    glScalef(length, 0.04, postWidth);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5, railY, 0);
    nextColor(DARK_GREY);
    glScalef(postWidth, 0.04, length);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5, railY, 0);
    nextColor(DARK_GREY);
    glScalef(postWidth, 0.04, length);
    glutSolidCube(1);
    glPopMatrix();
}


/*void drawTree(float x, float y, float z, float trunkHeight, float trunkWidth, float foliageSize) {
    glPushMatrix();
    glTranslatef(x, y + trunkHeight / 2, z);
    nextColor(BROWN);
    drawCube(0, 0, 0, trunkWidth);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y + trunkHeight + foliageSize / 2, z);
    nextColor(GREEN);
    drawCube(0, 0, 0, foliageSize);
    glPopMatrix();*/

void drawTree(float x, float y, float z, float trunkHeight, float trunkRadius, float leafHeight,
    GLuint trunkTextureID, GLuint leafTextureID) {
    glPushMatrix();
    glTranslatef(x, y, z); // Position the tree at (x, y, z)

    // Enable textures
    glEnable(GL_TEXTURE_2D);

    // Draw Tree Trunk with texture
    glBindTexture(GL_TEXTURE_2D, trunkTextureID);
    nextColor(BROWN); // Base color for the trunk

    GLUquadric* trunkQuad = gluNewQuadric();
    gluQuadricTexture(trunkQuad, GL_TRUE);
    gluQuadricNormals(trunkQuad, GLU_SMOOTH);

    glPushMatrix();
    glRotatef(-90, 1, 0, 0); // Make trunk vertical
    gluCylinder(trunkQuad, trunkRadius, trunkRadius, trunkHeight, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(trunkQuad); // Clean up after rendering trunk

    // Draw Tree Leaves with texture
    glBindTexture(GL_TEXTURE_2D, leafTextureID);
    nextColor(GREEN); // Base color for the leaves

    GLUquadric* leafQuad = gluNewQuadric();
    gluQuadricTexture(leafQuad, GL_TRUE);
    gluQuadricNormals(leafQuad, GLU_SMOOTH);

    // **Larger Cone Dimensions**
    float baseRadius = 3.5 * trunkRadius;  // Bigger base
    float middleRadius = 2.7 * trunkRadius;
    float topRadius = 2.0 * trunkRadius;    // Bigger top

    float baseHeight = leafHeight * 0.9;  // Increased height
    float middleHeight = leafHeight * 0.7;
    float topHeight = leafHeight * 0.5;

    // **Bottom Cone (Largest)**
    glPushMatrix();
    glTranslatef(0, trunkHeight, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leafQuad, baseRadius, 0, baseHeight, 20, 20);
    glPopMatrix();

    // **Middle Cone**
    glPushMatrix();
    glTranslatef(0, trunkHeight + baseHeight * 0.85, 0);  // Stack smoothly
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leafQuad, middleRadius, 0, middleHeight, 20, 20);
    glPopMatrix();

    // **Top Cone (Smallest)**
    glPushMatrix();
    glTranslatef(0, trunkHeight + baseHeight * 0.85 + middleHeight * 0.85, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leafQuad, topRadius, 0, topHeight, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(leafQuad); // Clean up

    glDisable(GL_TEXTURE_2D); // Disable texture mapping

    glPopMatrix();
}






void drawRoof(float x, float y, float z, float b, float l, float h, GLuint textureID) {
    float bh = b * 0.55;
    float lh = l * 0.55;
    float hh = h * 0.55;

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Define texture coordinates for the quads
    GLfloat texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Define vertices for the quads (main roof parts)
    GLfloat vertices[] = {
        x + lh, y + hh, z,
        x + lh, y - hh, z + bh,
        x - lh, y - hh, z + bh,
        x - lh, y + hh, z,
        x + lh, y + hh, z,
        x + lh, y - hh, z - bh,
        x - lh, y - hh, z - bh,
        x - lh, y + hh, z
    };

    // Set color for the quads
    nextColor(ORANGE);

    // Draw the quads
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glDrawArrays(GL_QUADS, 0, 8);

    // Define triangles for the end caps
    lh = l * 0.5;
    GLfloat triangles[] = {
        x + lh, y + hh, z,
        x + lh, y - hh, z - bh,
        x + lh, y - hh, z + bh,
        x - lh, y + hh, z,
        x - lh, y - hh, z + bh,
        x - lh, y - hh, z - bh
    };

    // Define texture coordinates for the triangles
    GLfloat triTexCoords[] = {
        0.5f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.5f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Set color for the triangles
    nextColor(WHITE);

    glVertexPointer(3, GL_FLOAT, 0, triangles);
    glTexCoordPointer(2, GL_FLOAT, 0, triTexCoords);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Clean up
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void handleHouseActions(const float k) {
#ifdef DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    cout << "X:" << X << " Y:" << Y << " Z:" << Z << endl;
    cout << "I:" << I << " J:" << J << " K:" << K << endl;
    cout << "-----------" << endl;
#endif

    glPushMatrix();
    applyTranslationRotation();
    glRotatef(houseRotationAngle, 0, 1, 0); // Rotate the house around the Y-axis
    nextColor(WHITE);
    drawCube(-0.5 * k, 0, 0, 1 * k, wallTexture);
    drawCube(0.5 * k, 0, 0, 1 * k, wallTexture);
    drawRoof(0, 0.7 * k, 0, 1.2 * k, 2 * k, 0.5 * k, roofTexture);

    nextColor(GREY);
    drawCube(0.5 * k, 0.65 * k, 0.3 * k, 0.15 * k, wallTexture);
    drawCube(0.5 * k, 0.8 * k, 0.3 * k, 0.15 * k, wallTexture);

    nextColor(BROWN);
    glRotatef(90, 0, k, 0);
    glTranslatef(0, 0, k + 0.01);
    glRectf(-0.15 * k, -0.5 * k, 0.15 * k, 0.1 * k);

    nextColor(YELLOW);
    glTranslatef(-0.1 * k, -0.2 * k, 0.008);
    glutSolidSphere(0.02f * k, 10, 10);
    glPopMatrix();

    drawWindow(k, 1, 1, windowTexture);
    drawWindow(k, -1, 1, windowTexture);
    drawWindow(k, 1, -1, windowTexture);
    drawWindow(k, -1, -1, windowTexture);

    glPushMatrix();
    applyTranslationRotation();
    nextColor(DARK_GREY);
    glRotatef(90, 0, k, 0);
    glTranslatef(0, 0, -k - 0.01);
    glRectf(-0.35 * k, -0.5 * k, 0.35 * k, 0.2 * k);
    glPopMatrix();

    drawFence(5.0, 0.5);
    // Adding trees
    drawTree(-2, -0.5, -2, 1.2, 0.1, 0.5, trunkTextureID, leafTextureID); // Increased size
    drawTree(2, -0.5, 2, 1.2, 0.1, 0.5, trunkTextureID, leafTextureID);   // Increased size

}

void processNormalKeys(unsigned char key, int x, int y) {
    float threshold = 0.1;
    switch (key) {
    case 'q':
    case 27:
        exit(0);
        break;

    case 'h':
        Z += threshold;
        break;
    case 'l':
        Z -= threshold;
        break;
    case 'k':
        X -= threshold;
        break;
    case 'j':
        X += threshold;
        break;
    case 'K':
        Y += threshold;
        break;
    case 'J':
        Y -= threshold;
        break;
    case '+':
        SCALE += threshold;
        break;
    case '-':
        if (SCALE > threshold)
            SCALE -= threshold;
        break;
    case 'c': // Rotate clockwise
        houseRotationAngle += 5.0f;
        break;
    case 'a': // Rotate anticlockwise
        houseRotationAngle -= 5.0f;
        break;
    }
    glutPostRedisplay(); // Redraw the scene after rotation
}

void processSpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        J -= 5.0f; // Rotate left
        break;
    case GLUT_KEY_RIGHT:
        J += 5.0f; // Rotate right
        break;
    case GLUT_KEY_UP:
        I -= 5.0f; // Rotate upward
        break;
    case GLUT_KEY_DOWN:
        I += 5.0f; // Rotate downward
        break;
    }
    glutPostRedisplay(); // Redraw the scene after rotation
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Light blue sky color
    glLoadIdentity();
    gluLookAt(6, 3, 2, 0, 0, 0, 0, 1, 0);

    // Update house rotation
    if (houseRotating) {
        houseRotationAngle += rotationSpeed;
        if (houseRotationAngle >= 360.0f) {
            houseRotationAngle -= 360.0f;
        }
    }

    handleHouseActions(SCALE);

    //glPushMatrix();
    //applyTranslationRotation();  // Apply global transformations
    //handleHouseActions(SCALE);
    //glPopMatrix();

    nextColor(DARK_GREEN);
    glBegin(GL_QUADS);
    glVertex3f(-5, -0.5, -5);
    glVertex3f(-5, -0.5, 5);
    glVertex3f(5, -0.5, 5);
    glVertex3f(5, -0.5, -5);
    glEnd();

    glutSwapBuffers();
}

void initTextures() {
    wallTexture = loadTexture("wall.jpg");
    roofTexture = loadTexture("roof.jpg");
    windowTexture = loadTexture("window.jpg");
    trunkTextureID = loadTexture("trunk.jpg");
    leafTextureID = loadTexture("leaf.jpg");
};

int main(int argc, char** argv) {

    int w = 1280;
    int h = 720;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow("3D Model with Lighting and Shading");
    initLighting();
    initTextures();
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutIdleFunc(renderScene);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMainLoop();

    return 1;

}