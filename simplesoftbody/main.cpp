#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

const GLint WIDTH = 800;
const GLint HEIGHT = 800;

const float DT = 0.002f;
const float MASS = 1.0f;
const float SPRING_STIFFNESS = 800.0f;
const float SPRING_DAMPING_FACTOR = 30.0f;
const float SELF_COLLISION_DISTANCE = 0.08f;

class PointMass {
public:
    float x;
    float y;
    float mass;
    vector<float> force;
    vector<float> velocity;
    
    PointMass(float x1,
              float y1,
              float mass1,
              vector<float> velocity1,
              vector<float> force1) {
        x = x1;
        y = y1;
        mass = mass1;
        velocity = velocity1;
        force = force1;
    }
};

class Environment {
public:
    vector<PointMass> particles;
    vector<PointMass> particlesInit;
    vector<float> groundLineVertices;
    vector<float> obstacle1;
    vector<vector<float>> obstacles;
    vector<vector<int>> adjacencyMatrix;

    Environment() {
        groundLineVertices = {
            -1.0f, -0.95f,
            1.0f, -0.95f
        };
        obstacle1 = {
            0.1f, -0.3f,
            0.8f, -0.2f
        };
        obstacles = {obstacle1};
        
        particles = {
            PointMass(0.0f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.3f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.2f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.1f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.0f, MASS, {0, 0}, {0, 0})
        };
        particlesInit = {
            PointMass(0.0f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.3f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.3f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.2f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.2f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.1f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.1f, MASS, {0, 0}, {0, 0}),

            PointMass(0.0f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.1f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.2f, 0.0f, MASS, {0, 0}, {0, 0}),
            PointMass(0.3f, 0.0f, MASS, {0, 0}, {0, 0})
        };
        adjacencyMatrix = {
            {0, 1, 0, 0,   1, 1, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0},
            {1, 0, 1, 0,   1, 1, 1, 0,   0, 0, 0, 0,   0, 0, 0, 0},
            {0, 1, 0, 1,   0, 1, 1, 1,   0, 0, 0, 0,   0, 0, 0, 0},
            {0, 0, 1, 0,   0, 0, 1, 1,   0, 0, 0, 0,   0, 0, 0, 0},
            
            {1, 1, 0, 0,   0, 1, 0, 0,   1, 1, 0, 0,   0, 0, 0, 0},
            {1, 1, 1, 0,   1, 0, 1, 0,   1, 1, 1, 0,   0, 0, 0, 0},
            {0, 1, 1, 1,   0, 1, 0, 1,   0, 1, 1, 1,   0, 0, 0, 0},
            {0, 0, 1, 1,   0, 0, 1, 0,   0, 0, 1, 1,   0, 0, 0, 0},
            
            {0, 0, 0, 0,   1, 1, 0, 0,   0, 1, 0, 0,   1, 1, 0, 0},
            {0, 0, 0, 0,   1, 1, 1, 0,   1, 0, 1, 0,   1, 1, 1, 0},
            {0, 0, 0, 0,   0, 1, 1, 1,   0, 1, 0, 1,   0, 1, 1, 1},
            {0, 0, 0, 0,   0, 0, 1, 1,   0, 0, 1, 0,   0, 0, 1, 1},
            
            {0, 0, 0, 0,   0, 0, 0, 0,   1, 1, 0, 0,   0, 1, 0, 0},
            {0, 0, 0, 0,   0, 0, 0, 0,   1, 1, 1, 0,   1, 0, 1, 0},
            {0, 0, 0, 0,   0, 0, 0, 0,   0, 1, 1, 1,   0, 1, 0, 1},
            {0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 1, 1,   0, 0, 1, 0},
        };
    }
    
    vector<float> getSpringForce(const int& i, const int& j) {
        float restLength = sqrt(pow(particlesInit[i].x - particlesInit[j].x, 2) +
                                pow(particlesInit[i].y - particlesInit[j].y, 2));
        
        float currentLength = sqrt(pow(particles[i].x - particles[j].x, 2) +
                                   pow(particles[i].y - particles[j].y, 2));
                
        vector<float> normalizedVector = {
            (particles[j].x - particles[i].x)/currentLength,
            (particles[j].y - particles[i].y)/currentLength
        };
        vector<float> velocityDifference = {
            particles[j].velocity[0] - particles[i].velocity[0],
            particles[j].velocity[1] - particles[i].velocity[1]
        };
        
        float dotProduct = normalizedVector[0] * velocityDifference[0] + normalizedVector[1] * velocityDifference[1];
        
        float springForce = SPRING_STIFFNESS * (currentLength - restLength);
        float dampingForce = SPRING_DAMPING_FACTOR * dotProduct;
        float totalForce = springForce + dampingForce;
        
        return {normalizedVector[0] * totalForce, normalizedVector[1] * totalForce};
    }
    
    void updateForce(const int& i){
        particles[i].force = {0, 0};
        
        // add gravity
        particles[i].force = {0, -9.8f * particles[i].mass};
        
        // add spring force
        for (int j = 0; j < adjacencyMatrix[i].size(); ++j) {
            if (adjacencyMatrix[i][j]) {
                vector<float> springForce = getSpringForce(i, j);
                particles[i].force[0] = particles[i].force[0] + springForce[0];
                particles[i].force[1] = particles[i].force[1] + springForce[1];
            }
        }
    }
    
    void eulerIntegrate(const int& i) {
        // euler integration - compute new velocity
        particles[i].velocity[0] = particles[i].velocity[0] + particles[i].force[0] * DT / particles[i].mass;
        particles[i].velocity[1] = particles[i].velocity[1] + particles[i].force[1] * DT / particles[i].mass;

        // euler integration - compute new position
        particles[i].x = particles[i].x + particles[i].velocity[0] * DT;
        particles[i].y = particles[i].y + particles[i].velocity[1] * DT;
    }
    
    bool collides(vector<float>& obstacle, PointMass& p) {
        // y = mx + b
        float m = (obstacle[3] - obstacle[1]) / (obstacle[2] - obstacle[0]);
        float b = obstacle[1] - m * obstacle[0];
        if (p.y < m * p.x + b && p.y > m * p.x + b - 0.05 && p.x >= obstacle[0] && p.x <= obstacle[2]) {
            return true;
        }
        return NULL;
    }
    
    void updateCollisions(const int& i) {
        if (particles[i].y < groundLineVertices[1]) {
            particles[i].y = groundLineVertices[1];
            particles[i].velocity = {particles[i].velocity[0], -particles[i].velocity[1]};
        }
        for (vector<float>& obstacle : obstacles) {
            if (collides(obstacle, particles[i])) {
                float m = (obstacle[3] - obstacle[1]) / (obstacle[2] - obstacle[0]);
                float b = obstacle[1] - m * obstacle[0];
                
                float mPrime = - 1/m;
                float bPrime = particles[i].y - mPrime * particles[i].x;
                
                float x = (bPrime - b) / (m - mPrime);
                float y = m * particles[i].x + b;
                
                float d = sqrt(pow(x - particles[i].x, 2) +
                               pow(y - particles[i].y, 2));
                
                vector<float> normalizedVector = {
                    (x - particles[i].x)/d,
                    (y - particles[i].y)/d
                };
                
                float dotProduct = particles[i].velocity[0] * normalizedVector[0] + particles[i].velocity[1] * normalizedVector[1];

                particles[i].x = x;
                particles[i].y = y;
                particles[i].velocity = {
                    particles[i].velocity[0] - 2 * dotProduct * normalizedVector[0],
                    particles[i].velocity[1] - 2 * dotProduct * normalizedVector[1]
                };
            }
        }
    }
    
    void updateSelfCollisions(const int& i) {
        for (int j = 0; j < particles.size(); ++j) {
            if (i != j) {
                float d = sqrt(pow(particles[j].x - particles[i].x, 2) +
                               pow(particles[j].y - particles[i].y, 2));
                
                if (d < SELF_COLLISION_DISTANCE && d != 0) {
                    vector<float> normalizedVector = {
                        (particles[j].x - particles[i].x)/d,
                        (particles[j].y - particles[i].y)/d
                    };
                    particles[i].x = particles[j].x - normalizedVector[0] * SELF_COLLISION_DISTANCE;
                    particles[i].y = particles[j].y - normalizedVector[1] * SELF_COLLISION_DISTANCE;

                    float dotProduct = particles[i].velocity[0] * normalizedVector[0] + particles[i].velocity[1] * normalizedVector[1];
                    
                    particles[i].velocity = {
                        particles[i].velocity[0] - 2 * dotProduct * normalizedVector[0],
                        particles[i].velocity[1] - 2 * dotProduct * normalizedVector[1]
                    };
                }
            }
        }
    }

    void step() {
        for (int i = 0; i < particles.size(); ++i) {
            updateForce(i);
            eulerIntegrate(i);
            updateSelfCollisions(i);
            updateCollisions(i);
        }
    }
};

GLfloat* vectorToArray(vector<float> vec) {
    GLfloat* vertexArray = new GLfloat[vec.size()];
    copy(vec.begin(), vec.end(), vertexArray);
    return vertexArray;
}

GLfloat* envPointsToGLPoints(Environment& env) {
    vector<float> particleVertices;
    for (PointMass p : env.particles) {
        particleVertices.push_back(p.x);
        particleVertices.push_back(p.y);
    }
    return vectorToArray(particleVertices);
}

GLfloat* envLinesToGLLines(Environment& env) {
    vector<float> lineVertices;

    for (int i = 0; i < env.adjacencyMatrix.size(); ++i) {
        for (int j = 0; j < env.adjacencyMatrix[i].size(); ++j) {
            if (env.adjacencyMatrix[i][j]) {
                lineVertices.push_back(env.particles[i].x);
                lineVertices.push_back(env.particles[i].y);
                lineVertices.push_back(env.particles[j].x);
                lineVertices.push_back(env.particles[j].y);
            }
        }
    }
    return vectorToArray(lineVertices);
}

GLfloat* envGroundToGLLines(vector<float>& endpoints) {
    return vectorToArray(endpoints);
}

int main() {
    glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "simple soft body", NULL, NULL);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if (!window) {
        cout << "Failed to create GLFW window." << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    if (GLEW_OK != glewInit()) {
        cout << "Failed to initialize GLEW." << endl;
        return -1;
    }
    
    Environment env = Environment();
    
    while (! glfwWindowShouldClose(window)) {
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_POINT_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        glPointSize(16);
        glVertexPointer(2, GL_FLOAT, 0, envPointsToGLPoints(env));
        glDrawArrays(GL_POINTS, 0, 16);
        
        glLineWidth(1);
        glVertexPointer(2, GL_FLOAT, 0, envLinesToGLLines(env));
        glDrawArrays(GL_LINES, 0, 168);

        glLineWidth(2);
        glVertexPointer(2, GL_FLOAT, 0, envGroundToGLLines(env.groundLineVertices));
        glDrawArrays(GL_LINES, 0, 2);

        glLineWidth(2);
        glVertexPointer(2, GL_FLOAT, 0, envGroundToGLLines(env.obstacle1));
        glDrawArrays(GL_LINES, 0, 2);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_POINT_SMOOTH);

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        env.step();
    }
    
    glfwTerminate();
    return 0;
}
