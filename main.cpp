// Butano libraries
#include "bn_core.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_info.h"
#include "common_variable_8x8_sprite_font.h"
#include "bn_sprite_items_paddle.h"
#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_bg.h"

// Additional libraries for string and math operations
#include "bn_string.h"
#include "bn_math.h"

int main()
{
    // Initialization
    bn::core::init();

    // Create a background at position 0,0
    bn::regular_bg_ptr bg = bn::regular_bg_items::bg.create_bg(0, 0);

    // Create the paddle at the bottom of the screen
    bn::sprite_ptr paddle = bn::sprite_items::paddle.create_sprite(0, 60);

    // Place the ball in the center of the screen
    bn::sprite_ptr ball = bn::sprite_items::ball.create_sprite(0, 0);

    // Initialize score and random object
    int score = 0;
    bool game_over = false;
    bn::random random;

    // Initialize text generator for the scoreboard
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 16> text_sprites;
    text_generator.generate(-80, -70, "Score: 0", text_sprites);

    // Ball movement variables
    int delta_x = 1;
    int delta_y = 1;

    // Cooldown period to prevent multiple score increments on one hit
    int hit_cooldown = 0;

    // Main game loop
    while (true)
    {
        // Check if game is over
        if (!game_over)
        {
            // Paddle movement using left and right keys, allowing full range across the 256-pixel width
            if (bn::keypad::left_held() && paddle.x() > -112)  // Adjusted constraint for left edge
            {
                paddle.set_x(paddle.x() - 2);
            }
            else if (bn::keypad::right_held() && paddle.x() < 112)  // Adjusted constraint for right edge
            {
                paddle.set_x(paddle.x() + 2);
            }

            // Ball movement
            ball.set_x(ball.x() + delta_x);
            ball.set_y(ball.y() + delta_y);

            // Decrease cooldown if it's active
            if (hit_cooldown > 0)
            {
                --hit_cooldown;
            }

            // Bounce ball off the top, left, and right edges
            if (ball.x() < -112 || ball.x() > 112)
            {
                delta_x = -delta_x;
            }
            if (ball.y() < -70)
            {
                delta_y = -delta_y;
            }

            // Check for collision with the paddle
            if (hit_cooldown == 0 && ball.y() > 52 && bn::abs(ball.x() - paddle.x()) < 16)
            {
                delta_y = -delta_y;  // Bounce the ball upwards
                score++;             // Increment score
                hit_cooldown = 20;   // Set cooldown period

                // Update the scoreboard
                text_sprites.clear();
                bn::string<32> txt_score = "Score: " + bn::to_string<32>(score);
                text_generator.generate(-80, -70, txt_score, text_sprites);
            }

            // End game if the ball touches the bottom edge
            if (ball.y() > 70)
            {
                game_over = true;
                delta_x = 0;
                delta_y = 0;  // Stop the ball movement

                text_sprites.clear();

                // Create the final game over message with the score
                bn::string<32> game_over_message = "Game Over - Score " + bn::to_string<32>(score);

                // Display the message at a centered position on the screen
                text_generator.generate(-80, -70, game_over_message, text_sprites);
            }
        }
        else
        {
            // Restart game if Start button is pressed
            if (bn::keypad::start_pressed())
            {
                // Reset positions and variables
                ball.set_position(0, 0);
                paddle.set_position(0, 60);
                delta_x = 1;
                delta_y = 1;
                score = 0;
                hit_cooldown = 0;  // Reset cooldown
                game_over = false;

                // Clear and reset the score display
                text_sprites.clear();
                text_generator.generate(-80, -70, "Score: 0", text_sprites);
            }
        }

        // Update the display
        bn::core::update();
    }
}
