package gui;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

public abstract class ResizableCanvas extends Canvas {

    public ResizableCanvas() {
        // Redraw canvas when size changes.
        widthProperty().addListener(evt -> draw());
        heightProperty().addListener(evt -> draw());
    }
    abstract void draw();
    @Override
    public boolean isResizable() {return true;}
    @Override
    public double prefWidth(double height) {return getWidth();}
    @Override
    public double prefHeight(double width) {return getHeight();}
}
