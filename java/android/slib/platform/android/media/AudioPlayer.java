package slib.platform.android.media;

import android.media.MediaPlayer;

import slib.platform.android.Logger;
import slib.platform.android.SlibActivity;

public class AudioPlayer {

    public static MediaPlayer createMedia(final SlibActivity context, final String url, final long instance)  {
        try {
            MediaPlayer player = new MediaPlayer();
            player.setDataSource(url);
            player.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mp) {
                    nativeOnCompleted(instance);
                }
            });
            player.prepareAsync();
            player.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                @Override
                public void onPrepared(MediaPlayer mp) {
                    nativeOnPrepared(instance);
                }
            });
            return player;
        } catch (Exception e) {
            Logger.exception(e);
            return null;
        }
    }

    public static void start(final SlibActivity context, MediaPlayer mp) {
        try {
            if (mp != null) {
                mp.start();
            }
        } catch (Exception e) {
            Logger.exception(e);
        }
    }

    public static void pause(final SlibActivity context, MediaPlayer mp) {
        try {
            if (mp != null) {
                mp.pause();
            }
        } catch (Exception e) {
            Logger.exception(e);
        }
    }

    public static void stop(final SlibActivity context, MediaPlayer mp) {
        try {
            if (mp != null) {
                mp.stop();
                mp.release();
            }
        } catch (Exception e) {
            Logger.exception(e);
        }
    }

    public static boolean isPlaying(final SlibActivity context, MediaPlayer mp) {
        try {
            if (mp != null) {
                return mp.isPlaying();
            }
        } catch (Exception e) {
            Logger.exception(e);
        }
        return false;
    }

    private static native void nativeOnCompleted(long instance);
    private static native void nativeOnPrepared(long instance);
}