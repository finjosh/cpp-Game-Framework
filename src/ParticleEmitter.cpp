#include "ParticleEmitter.hpp"

ParticleEmitter::Particle::Particle(const b2Vec2& pos, const b2Vec2& vel, const float& rotation, const sf::Color& color) 
    : _position(pos), _velocity(vel), _rotation(rotation), _color(color) {}

void ParticleEmitter::Particle::setPosition(const b2Vec2& position)
{
    _position = position;
}

b2Vec2 ParticleEmitter::Particle::getPosition() const
{
    return _position;
}

void ParticleEmitter::Particle::setVelocity(const b2Vec2& velocity)
{
    _velocity = velocity;
}

b2Vec2 ParticleEmitter::Particle::getVelocity() const
{
    return _velocity;
}

void ParticleEmitter::Particle::setRotation(const float& rot)
{
    _rotation = rot;
}

float ParticleEmitter::Particle::getRotation() const
{
    return _rotation;
}

float ParticleEmitter::Particle::getLifetime() const
{
    return _lifetime;
}

sf::Color ParticleEmitter::Particle::getColor() const
{
    return _color;
}

void ParticleEmitter::Particle::setColor(const sf::Color& color)
{
    _color = color;
}

void ParticleEmitter::Particle::setAlpha(const int8_t& alpha)
{
    _color.a = alpha;
}

void ParticleEmitter::Particle::move(const b2Vec2& vec)
{
    _position.x += vec.x;
    _position.y += vec.y;
}

void ParticleEmitter::Particle::draw(sf::Shape* shape, sf::RenderWindow& window)
{
    shape->setPosition({_position.x * PIXELS_PER_METER, _position.y * PIXELS_PER_METER});
    shape->setRotation(_rotation);
    shape->setFillColor(_color);
    window.draw(*shape);
}

void ParticleEmitter::Particle::update(const float& deltaTime)
{
    _position.x += _velocity.x * deltaTime;
    _position.y += _velocity.y * deltaTime;
    _lifetime += deltaTime;
}

ParticleEmitter::ParticleEmitter(sf::Shape* shape) : _particleShape(shape)
{}

ParticleEmitter::ParticleEmitter(sf::Shape* shape, const b2Vec2& pos, 
                    const float& vel, const float& rot, const float& spawnRate,
                    const float& lifetime, const int& spawnAmount, const float& fadeOutTime, 
                    const float& spread, const sf::Color& randomColor)
    : _particleShape(shape), _velocity(vel), _spawnRate(spawnRate), _lifetime(lifetime), 
    _spawnAmount(spawnAmount), _fadeOutTime(fadeOutTime), _spread(spread), _randomColor(randomColor)
{
    this->setPosition(pos);
    this->setRotation(rot);
    
    if (shape != nullptr)
    {
        _defaultColor = shape->getFillColor();
    }
}

void ParticleEmitter::removeShape()
{
    _particleShape = nullptr;
}

void ParticleEmitter::setShape(sf::Shape* shape)
{
    _particleShape = shape;
}

const sf::Shape* ParticleEmitter::getShape() const
{
    return _particleShape;
}

sf::Shape* ParticleEmitter::getShape()
{
    return _particleShape;
}

bool ParticleEmitter::getSpawning() const
{
    return _spawning;
}

void ParticleEmitter::setSpawning(const bool& value)
{
    _spawning = value;
}

float ParticleEmitter::getSpawnRate() const
{
    return _spawnRate;
}

void ParticleEmitter::setSpawnRate(const float& value)
{
    _spawnRate = value;
}

float ParticleEmitter::getRandomSpawnRate() const
{
    return _randomSpawnRate;
}

void ParticleEmitter::setRandomSpawnRate(const float& value)
{
    _randomSpawnRate = value;
}

int ParticleEmitter::getSpawnAmount() const
{
    return _spawnAmount;
}

void ParticleEmitter::setSpawnAmount(const int& value)
{
    _spawnAmount = value;
}

unsigned int ParticleEmitter::getRandomSpawnAmount() const
{
    return _randomSpawnAmount;
}

void ParticleEmitter::setRandomSpawnAmount(const unsigned int& value)
{
    _randomSpawnAmount = std::min(std::max(value, 1U), (unsigned int)RAND_MAX);
}

float ParticleEmitter::getSpread() const
{
    return _spread;
}

void ParticleEmitter::setSpread(const float& value)
{
    _spread = value;
}

float ParticleEmitter::getRandomRotation() const
{
    return _randomRotation;
}

void ParticleEmitter::setRandomRotation(const float& value)
{
    _randomRotation = value;
}

float ParticleEmitter::getLifetime() const
{
    return _lifetime;
}

void ParticleEmitter::setLifetime(const float& value)
{
    _lifetime = value;
}

float ParticleEmitter::getFadeOutTime() const
{
    return _fadeOutTime;
}

void ParticleEmitter::setFadeOutTime(const float& value)
{
    _fadeOutTime = value;
}

float ParticleEmitter::getVelocity() const
{
    return _velocity;
}

void ParticleEmitter::setVelocity(const float& value)
{
    _velocity = value;
}

void ParticleEmitter::setRandomColor(const sf::Color& color)
{
    _randomColor = color;
}

sf::Color ParticleEmitter::getRandomColor() const
{
    return _randomColor;
}

void ParticleEmitter::setDefaultColor(const sf::Color& color)
{
    _defaultColor = color;
}

sf::Color ParticleEmitter::getDefaultColor() const
{
    return _defaultColor;
}

void ParticleEmitter::emit()
{
    for (unsigned int i = 0; i < _spawnAmount - (rand()%_randomSpawnAmount); i++)
    {
        float spread = (randX(_spread) - _spread/2)*b2_pi/180;
        float rotation = randX(_randomRotation);
        sf::Color color(_defaultColor.r + rand()%(_randomColor.r+1), _defaultColor.g + rand()%(_randomColor.g+1),
                        _defaultColor.b + rand()%(_randomColor.b+1), _defaultColor.a + rand()%(_randomColor.a+1));
        auto polar = std::polar<float>(_velocity, this->getRotation() + spread);

        _particles.push_back({this->getPosition(), 
                            {polar.real(), polar.imag()}, 
                            rotation, color});
    }
}

void ParticleEmitter::Update(const float& deltaTime)
{
    for (std::list<Particle>::iterator particle = _particles.begin(); particle != _particles.end(); particle++)
    {
        particle->update(deltaTime);
        if (particle->getLifetime() >= _lifetime)
        {
            auto temp = particle;
            particle--;
            _particles.erase(temp);
        }
        if (particle->getLifetime() >= _lifetime - _fadeOutTime)
        {
            particle->setAlpha(_defaultColor.a*((_lifetime - particle->getLifetime())/_fadeOutTime));
        }
    }
    
    if (!_spawning)
        return;

    _spawnTimer += deltaTime + randX(_randomSpawnRate);
    if (_spawnTimer >= _spawnRate)
    {
        _spawnTimer = 0.f;
        this->emit();
    }
}

void ParticleEmitter::Draw(sf::RenderWindow& window)
{
    if (_particleShape == nullptr)
        return;

    for (auto particle: _particles)
    {
        particle.draw(_particleShape, window);
    }
}

float ParticleEmitter::randX(const float& x)
{
    return ((float)rand() / (float)RAND_MAX) * x;
}

void ParticleEmitter::clear()
{
    _particles.clear();
}
