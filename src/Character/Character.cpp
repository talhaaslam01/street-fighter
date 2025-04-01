#include "Character.h"
#include "raymath.h"

Character::Character(const int gameWidth, const int gameHeight, const bool isPlayer1,
                     const Stage &stage, const Camera2D &camera)
    : m_isPlayer1(isPlayer1),
      m_position({}),
      m_size({DEFAULT_CHARACTER_WIDTH, DEFAULT_CHARACTER_HEIGHT}),
      m_velocity({0.0f, 0.0f}),
      m_stage(stage),
      m_camera(camera),
      m_currentState(CharacterState::STANDING),
      m_currentAnimation(Action::STANDING),
      m_currentAnimFrame(0),
      m_animTimer(0.0f),
      m_facingDirection(isPlayer1 ? 1 : -1)
{
    // set initial character position
    m_position.y = m_stage.getFloorY();

    // X position depends on player number
    float xPosScreenCoord;
    if (m_isPlayer1)
    {
        xPosScreenCoord = gameWidth / STARTING_POSITION_FACTOR_X;
    }
    else
    {
        xPosScreenCoord = gameWidth - m_size.x - (gameWidth / STARTING_POSITION_FACTOR_X);
    }
    m_position.x = GetScreenToWorld2D({xPosScreenCoord, 0.0f}, m_camera).x;
}

void Character::initialize()
{
    m_spriteSheetManager.loadSpriteSheet("ryu");
    m_spriteSheetManager.loadSpriteSheetMetaData("ryu");
    m_animationsManager.loadAnimationMetaData("ryu");
}

void Character::update(float dt)
{
    // Update character logic
    m_animTimer += dt;

    const AnimationGroup &animationGroup = m_animationsManager.getAnimationGroup(m_currentAnimation);
    if (m_animTimer >= animationGroup.frames[m_currentAnimFrame].frames / 60.0f)
    {
        m_animTimer = 0.0f;
        m_currentAnimFrame += 1;

        if (m_currentAnimFrame >= animationGroup.frames.size())
        {
            if (animationGroup.loopStartFrameIndex >= 0)
            {
                m_currentAnimFrame = animationGroup.loopStartFrameIndex;
            }
            else
            {
                m_currentAnimFrame = animationGroup.frames.size() - 1;
            }
        }
    }

    // Handle input for simple state changes
    // TEMPORARY
    if (m_currentState == CharacterState::STANDING)
    {
        if (IsKeyDown(m_isPlayer1 ? KEY_RIGHT : KEY_D))
        {
            m_currentState = CharacterState::WALKING_FORWARD;
            m_currentAnimation = Action::WALKING_FORWARD;
            m_currentAnimFrame = 0;
            m_animTimer = 0.0f;
        }
        else if (IsKeyDown(m_isPlayer1 ? KEY_LEFT : KEY_A))
        {
            m_currentState = CharacterState::WALKING_BACKWARD;
            m_currentAnimation = Action::WALKING_BACKWARD;
            m_currentAnimFrame = 0;
            m_animTimer = 0.0f;
        }
    }
    else if (m_currentState == CharacterState::WALKING_FORWARD)
    {
        if (!IsKeyDown(m_isPlayer1 ? KEY_RIGHT : KEY_D))
        {
            m_currentState = CharacterState::STANDING;
            m_currentAnimation = Action::STANDING;
            m_currentAnimFrame = 0;
            m_animTimer = 0.0f;
        }
    }
    else if (m_currentState == CharacterState::WALKING_BACKWARD)
    {
        if (!IsKeyDown(m_isPlayer1 ? KEY_LEFT : KEY_A))
        {
            m_currentState = CharacterState::STANDING;
            m_currentAnimation = Action::STANDING;
            m_currentAnimFrame = 0;
            m_animTimer = 0.0f;
        }
    }
}

void Character::fixedUpdate(float fixedDt)
{
    // Update character physics
    // Handle movement
    if (m_currentState == CharacterState::WALKING_FORWARD)
    {
        m_velocity.x = MOVE_SPEED * m_facingDirection;
    }
    else if (m_currentState == CharacterState::WALKING_BACKWARD)
    {
        m_velocity.x = -MOVE_SPEED * m_facingDirection;
    }
    else
    {
        m_velocity.x = 0;
    }

    // Apply velocity
    m_position.x += m_velocity.x * fixedDt;

    // Keep character within stage bounds
    const Vector4 &bounds = m_stage.getBounds();
    float stageLeft = bounds.x;
    float stageRight = bounds.z;

    if (m_position.x < stageLeft)
    {
        m_position.x = stageLeft;
    }
    else if (m_position.x + m_size.x > stageRight)
    {
        m_position.x = stageRight - m_size.x;
    }
}

void Character::render()
{
    // Get current animation frame
    const Texture2D &spriteSheet = m_spriteSheetManager.getSpriteSheet();
    const Sprite &sprite = m_spriteSheetManager.getSprite(m_currentAnimation, m_currentAnimFrame);
    const AnimationFrame &animFrame = m_animationsManager.getAnimationGroup(m_currentAnimation).frames[m_currentAnimFrame];
    const Rectangle &sourceRect = sprite.source;
    const Vector2 &pivot = sprite.pivot;

    Vector2 drawPos = {
        m_position.x - pivot.x + animFrame.offset.x,
        m_position.y - pivot.y + animFrame.offset.y,
    };

    Rectangle src = {sourceRect.x, sourceRect.y, m_facingDirection * sourceRect.width, sourceRect.height};
    Rectangle dest = {drawPos.x, drawPos.y, sprite.source.width, sprite.source.height};

    DrawTexturePro(spriteSheet, src, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);

    // Debug render
    Color color = m_isPlayer1 ? BLUE : MAGENTA;
    DrawRectangleLines(
        static_cast<int>(roundf(m_position.x)),
        static_cast<int>(roundf(m_position.y - m_size.y)),
        static_cast<int>(roundf(m_size.x)),
        static_cast<int>(roundf(m_size.y)),
        color);

    DrawCircle(static_cast<int>(roundf(m_position.x)), static_cast<int>(roundf(m_position.y)), 1, RED);
}

void Character::imGuiDebugRender()
{
    const char *name = m_isPlayer1 ? "Player 1" : "Player 2";

    ImGui::Begin(name);
    {
        ImGui::SliderFloat("Pos x", &m_position.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("Pos y", &m_position.y, -1000.0f, 1000.0f);
        ImGui::Text("Position: (%.2f, %.2f)", m_position.x, m_position.y);
        ImGui::Text("Velocity: (%.2f, %.2f)", m_velocity.x, m_velocity.y);
        if (ImGui::Button("Facing Direction toggle"))
        {
            m_facingDirection = (m_facingDirection == 1) ? -1 : 1;
        }
        ImGui::Text("Facing Direction: %d", m_facingDirection);
    }
    ImGui::End();
}
