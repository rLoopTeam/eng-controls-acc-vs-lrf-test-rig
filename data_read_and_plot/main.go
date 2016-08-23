package main

import (
    "fmt"
    "github.com/tarm/goserial"
    "bufio"
    "io"
    "os"
    "strings"
)

//opens a serial (USB) port. if a "port_number" is send, that port will be ignored
func openPort(port_nr ...int) (io.ReadWriteCloser, int) {
    
    //ACM port number changes sometimes for some reason.. try a bunch!
    for i := 0; i<10; i++ {

        //we dont want to open the same port number twice
        if (len(port_nr) > 0 && i==port_nr[0]){
            continue
        }
    
        //try opening serial port i
        ttystring := fmt.Sprintf("/dev/ttyACM%d", i)
        config := &serial.Config{Name: ttystring, Baud: 9600}
        usb, err := serial.OpenPort(config)
        
        if err != nil {
                fmt.Println(err)
        } else {
            fmt.Println("Opened ", ttystring)
            return usb, i
        }
    }
    panic("Could not open serial device 0-10") //kill the program if we cant find a serial port to open   
}

//figure out which port is the laser and which is accelerometer
func findLaser(reader_1 *bufio.Reader, reader_2 *bufio.Reader) (*bufio.Reader, *bufio.Reader) {

    data, err := reader_1.ReadBytes('\n')
    if err != nil {
        fmt.Println(err)
    }

    //grab a sample from one sensor and see how many values it has,
    //we know the accelerometer should have alot more than 2 values
    fmt.Println(string(data))
    split := strings.Split( string(data),",")
    if len(split) > 2 {
        return reader_1, reader_2
    } else {
        return reader_2, reader_1
    }
}

//this thread syncs up the data between the sensors if they are sending at diffrent rates.
func fileWriter(laser_channel_send chan bool, acc_channel_send chan bool, laser_channel_data chan string, acc_channel_data chan string, file *os.File){
    var laser_data, acc_data string

    for{
        //send some data on the channels to let both sensors send one value
        laser_channel_send <- true
        acc_channel_send <- true

        //read the values. fileWriter will block here untill it gets one value from both sensors.
        //this means fileWriter will automatically adjust to the sensor with the slowest data rate.
        //excess data from the faster sensor is discarded
        laser_data = <- laser_channel_data
        acc_data = <- acc_channel_data

        //print and write to file
        fmt.Println(laser_data)
        fmt.Println(acc_data)
        file.WriteString(acc_data + "," + laser_data + "\n")
    }

}

//reads sensor data and discards excess values
func readSensor(reader *bufio.Reader, channel_in chan bool, channel_out chan string){

    for{
        //read one value from sensor
        data, err := reader.ReadBytes('\n')
        if err != nil {
            panic(err)
        }

        //check if we should send this value to the file writer
        select {

            //this is basically an if("channel has some data"). also removes the data from channel, so we can only write send sensor value per channel_in data received. 
            //this is what creates the synchronization, think of the data as a "permission to send one value"-token.
            //what we send on the _in channel doesnt matter, only that we send "something".
            //in this case we send booleans but could be anything. 
            case  <- channel_in: 
                data = data[:len(data)-2] //remove "\n\r from sensor value"
                channel_out <- string(data) //send sensor value to file writer thread
            default:
                //if there was no data on the channel the "select" will go into the default case, and we discard the sensor value. aka DO NOTHING :D    
        }
    }
}

func main() {

    //these channels are used by fileWriter to synchronize the sensor data-rates
    //the "0" means there can only be 1 value on the channel at any one time, aka "unbuffered" channel
    laser_channel_in := make(chan bool, 0)
    acc_channel_in := make(chan bool, 0)

    //these are used to send sensor values to the file writer
    laser_channel_data := make(chan string, 0)
    acc_channel_data := make(chan string, 0)

    //open two usb ports. we use i to make sure we dont open the same port twice!
    usb_1, port_nr := openPort()
    usb_2, _ := openPort(port_nr)
    
    //make 2 buffered readers from serial port readers. buffered readers are easier to work with
    reader_1 := bufio.NewReader(usb_1)
    reader_2 := bufio.NewReader(usb_2)

    //figure out which port is laser and which is accelerometer
    accelerometer, laser := findLaser(reader_1, reader_2)

    //reader from the keyboard
    key_reader := bufio.NewReader(os.Stdin)

    fmt.Println("Ready to record, enter file name?")  

    file_name,_ := key_reader.ReadString('\n')
    file_name = file_name[:len(file_name)-1] // remove "\n"
    //we need a default file name if nothing was entered
    if len(file_name) == 0 {
        file_name = fmt.Sprint("data")
    }
    file_name += ".csv"
    fmt.Println("Recording..")
    fmt.Println("Filename: ", file_name)

    //file_name = "/home/nebu/python/rchart/" + file_name //change this to where to save files on your computer

    //remove old file and create a new one
    _ = os.Remove(file_name)
    file, err := os.Create(file_name)
    if err != nil {
        panic(err)
    }

    //write the header with column names
    file.WriteString("X_acceleration,Y_acceleration,X_velocity,Y_velocity,X_distance,Y_distance,Kalman_1_acc,Kalman_2_acc,Kalman_X_vel,Kalman_Y_vel,Kalman_X_dist,Kalman_Y_dist,dt,laser_vel,laser_dist\n")

    //start file writer and sensor reader threads. "go" command runs a function in a separate thread! (technically a separate "go routine", which is "almost" a thread)
    go fileWriter(laser_channel_in, acc_channel_in, laser_channel_data, acc_channel_data, file)
    go readSensor(laser, laser_channel_in, laser_channel_data)
    go readSensor(accelerometer, acc_channel_in, acc_channel_data)

    _,_ = key_reader.ReadString('\n')
    fmt.Println("Stopped recording")
    fmt.Println("-----------------\n")
}