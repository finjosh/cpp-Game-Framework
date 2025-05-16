#include "Particles/ParticleEmitter.hpp"

ParticleEmitter::Particle::Particle(const Vector2& pos, const Vector2& vel, sf::Angle rotation, sf::Color color) 
    : m_position(pos), m_velocity(vel), m_rotation(rotation), m_color(color) {}

void ParticleEmitter::Particle::setPosition(const Vector2& position)
{
    m_position = position;
}

Vector2 ParticleEmitter::Particle::getPosition() const
{
    return m_position;
}

void ParticleEmitter::Particle::setVelocity(const Vector2& velocity)
{
    m_velocity = velocity;
}

Vector2 ParticleEmitter::Particle::getVelocity() const
{
    return m_velocity;
}

void ParticleEmitter::Particle::setRotation(sf::Angle angle)
{
    m_rotation = angle;
}

sf::Angle ParticleEmitter::Particle::getRotation() const
{
    return m_rotation;
}

float ParticleEmitter::Particle::getLifetime() const
{
    return m_lifetime;
}

sf::Color ParticleEmitter::Particle::getColor() const
{
    return m_color;
}

void ParticleEmitter::Particle::setColor(sf::Color color)
{
    m_color = color;
}

void ParticleEmitter::Particle::setAlpha(int8_t alpha)
{
    m_color.a = alpha;
}

void ParticleEmitter::Particle::move(const Vector2& vec)
{
    m_position.x += vec.x;
    m_position.y += vec.y;
}

void ParticleEmitter::Particle::draw(sf::Shape* shape, sf::RenderTarget* target)
{
    shape->setPosition({m_position.x * PIXELS_PER_METER, m_position.y * PIXELS_PER_METER});
    shape->setRotation(m_rotation);
    shape->setFillColor(m_color);
    target->draw(*shape);
}

void ParticleEmitter::Particle::update(float deltaTime)
{
    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_lifetime += deltaTime;
}

ParticleEmitter::ParticleEmitter(sf::Shape* shape) : m_particleShape(shape)
{}

ParticleEmitter::ParticleEmitter(sf::Shape* shape, const Vector2& pos, 
                    float vel, Rotation rotation, float spawnRate,
                    float lifetime, int spawnAmount, float fadeOutTime, 
                    float spread, sf::Color randomColor)
    : m_particleShape(shape), m_velocity(vel), m_spawnRate(spawnRate), m_lifetime(lifetime), 
    m_spawnAmount(spawnAmount), m_fadeOutTime(fadeOutTime), m_spread(spread), m_randomColor(randomColor)
{
    this->setPosition(pos);
    this->setRotation(rotation);
    
    if (shape != nullptr)
    {
        m_defaultColor = shape->getFillColor();
    }
}

void ParticleEmitter::removeShape()
{
    m_particleShape = nullptr;
}

void ParticleEmitter::setShape(sf::Shape* shape)
{
    m_particleShape = shape;
}

const sf::Shape* ParticleEmitter::getShape() const
{
    return m_particleShape;
}

sf::Shape* ParticleEmitter::getShape()
{
    return m_particleShape;
}

bool ParticleEmitter::isSpawning() const
{
    return m_spawning;
}

void ParticleEmitter::setSpawning(bool value)
{
    m_spawning = value;
}

float ParticleEmitter::getSpawnRate() const
{
    return m_spawnRate;
}

void ParticleEmitter::setSpawnRate(float value)
{
    m_spawnRate = value;
}

float ParticleEmitter::getRandomSpawnRate() const
{
    return m_randomSpawnRate;
}

void ParticleEmitter::setRandomSpawnRate(float value)
{
    m_randomSpawnRate = value;
}

int ParticleEmitter::getSpawnAmount() const
{
    return m_spawnAmount;
}

void ParticleEmitter::setSpawnAmount(int value)
{
    m_spawnAmount = value;
}

unsigned int ParticleEmitter::getRandomSpawnAmount() const
{
    return m_randomSpawnAmount;
}

void ParticleEmitter::setRandomSpawnAmount(unsigned int value)
{
    m_randomSpawnAmount = std::min(std::max(value, 1U), (unsigned int)RAND_MAX);
}

float ParticleEmitter::getSpread() const
{
    return m_spread;
}

void ParticleEmitter::setSpread(float value)
{
    m_spread = value;
}

float ParticleEmitter::getRandomRotation() const
{
    return m_randomRotation;
}

void ParticleEmitter::setRandomRotation(float value)
{
    m_randomRotation = value;
}

float ParticleEmitter::getLifetime() const
{
    return m_lifetime;
}

void ParticleEmitter::setLifetime(float value)
{
    m_lifetime = value;
}

float ParticleEmitter::getFadeOutTime() const
{
    return m_fadeOutTime;
}

void ParticleEmitter::setFadeOutTime(float value)
{
    m_fadeOutTime = value;
}

float ParticleEmitter::getVelocity() const
{
    return m_velocity;
}

void ParticleEmitter::setVelocity(float value)
{
    m_velocity = value;
}

float ParticleEmitter::getRandomVelocity() const
{
    return m_randomVelocity;
}

void ParticleEmitter::setRandomVelocity(float value)
{
    m_randomVelocity = value;
}

void ParticleEmitter::setRandomColor(sf::Color color)
{
    m_randomColor = color;
}

sf::Color ParticleEmitter::getRandomColor() const
{
    return m_randomColor;
}

void ParticleEmitter::setDefaultColor(sf::Color color)
{
    m_defaultColor = color;
}

sf::Color ParticleEmitter::getDefaultColor() const
{
    return m_defaultColor;
}

void ParticleEmitter::emit()
{
    for (unsigned int i = 0; i < m_spawnAmount - (rand()%m_randomSpawnAmount); i++)
    {
        Rotation direction((randX(m_spread) - m_spread/2)*PI/180); // starting with spread
        direction += Object::getGlobalRotation(); // adding the current rotation
        float rotation = randX(m_randomRotation);
        sf::Color color(m_defaultColor.r + rand()%(m_randomColor.r+1), m_defaultColor.g + rand()%(m_randomColor.g+1),
                        m_defaultColor.b + rand()%(m_randomColor.b+1), m_defaultColor.a + rand()%(m_randomColor.a+1));
        float velocity = m_velocity + randX(m_randomVelocity*2) - m_randomVelocity;

        m_particles.push_back({this->getGlobalPosition(), 
                            {direction.cos * velocity, direction.sin * velocity}, 
                            sf::degrees(rotation), color});
    }
}

void ParticleEmitter::Update(float deltaTime)
{
    for (std::list<Particle>::iterator particle = m_particles.begin(); particle != m_particles.end(); particle++)
    {
        particle->update(deltaTime);
        if (particle->getLifetime() >= m_lifetime)
        {
            auto temp = particle;
            particle--;
            m_particles.erase(temp);
            if (particle == m_particles.end())
                break;
        }
        else if (particle->getLifetime() >= m_lifetime - m_fadeOutTime)
        {
            particle->setAlpha(m_defaultColor.a*((m_lifetime - particle->getLifetime())/m_fadeOutTime));
        }
    }
    
    if (!m_spawning)
        return;

    m_spawnTimer += deltaTime + randX(m_randomSpawnRate);
    if (m_spawnTimer >= m_spawnRate)
    {
        m_spawnTimer = 0.f;
        this->emit();
    }
}

void ParticleEmitter::Draw(sf::RenderTarget* target, const Transform& parentTransform)
{
    if (m_particleShape == nullptr)
        return;

    for (auto particle: m_particles)
    {
        particle.draw(m_particleShape, target);
    }
}

float ParticleEmitter::randX(float x)
{
    return ((float)rand() / (float)RAND_MAX) * x;
}

void ParticleEmitter::clear()
{
    m_particles.clear();
}
