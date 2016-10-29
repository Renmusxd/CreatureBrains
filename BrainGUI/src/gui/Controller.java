package gui;


import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.scene.canvas.Canvas;
import model.Model;

public class Controller {
    public final long GUI_UPDATE = 33;
    public final Model m;
    public boolean running;

    public Controller(Model m) {
        this.m = m;
        running = true;
    }

    public void startUpdateThread(){
        Task task = new Task<Void>(){
            @Override
            protected Void call() throws Exception {
                System.out.println("here");
                while (running) {
                    try{m.update();}
                    catch (Exception ignored){}
                    Platform.runLater(() -> {
                        if (Main.map!=null) Main.map.draw();
                    });
                    Thread.sleep(GUI_UPDATE);
                }
                return null;
            }
        };
        Thread t = new Thread(task);
        t.setName("Update thread");
        t.setDaemon(true);
        t.start();
    }

    public void stopUpdateThread(){
        running = false;
    }
}
