package by.kostahka.aftertime_imprisonment;

import org.libsdl.app.SDLActivity;

public class GameActivity extends SDLActivity{
    protected String getMainFunction() {
        return "Game_main";
    }

    protected String[] getLibraries() {
        return new String[] {
                "c++_shared",
                // "SDL3",
                // "SDL3_image",
                // "SDL3_mixer",
                // "SDL3_net",
                // "SDL3_ttf",
                "special-game"
        };
    }
}
