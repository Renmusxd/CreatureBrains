package gui;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import model.Model;

public class Main extends Application {

    static Model m;
    static Controller c;
    static CritterMapCanvas map;

    @Override
    public void start(Stage primaryStage) throws Exception{
        Parent root = FXMLLoader.load(getClass().getResource("gui.fxml"));
        primaryStage.setTitle("Brain GUI");
        primaryStage.setScene(new Scene(root, 750, 500));
        primaryStage.setMinHeight(500); primaryStage.setMinWidth(750);

        Pane centerPane = (StackPane) root.lookup("#centerpane");
        map = new CritterMapCanvas(m);
        map.widthProperty().bind(centerPane.widthProperty());
        map.heightProperty().bind(centerPane.heightProperty());
        centerPane.getChildren().add(map);

        primaryStage.show();

        c.startUpdateThread();
    }


    public static void main(String[] args) {
        m = new Model("localhost",1708);
        c = new Controller(m);
        launch(args);
    }
}
