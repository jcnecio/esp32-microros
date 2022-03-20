
import React from 'react';
import type {Node} from 'react';
import {
  StyleSheet,
  Text,
  View,
  TouchableOpacity
} from 'react-native';


const App: () => Node = () => {

  const handleClick = (pin) => {
    const HOST = "http://192.168.68.117:5000?dir="
    fetch(`${HOST}${pin}`).then(res => res);
  }

  return (
    <View style={styles.container}>
      <View style={styles.row}>
        <View style={styles.col}>
        </View>
        <View style={styles.col}>
          <TouchableOpacity style={styles.button} onPress={() => {handleClick(1)}}><Text>FORWARD</Text></TouchableOpacity>
        </View>
        <View style={styles.col}>
        </View>
      </View>
      <View style={styles.row}>
        <View style={styles.col}>
          <TouchableOpacity style={styles.button} onPress={() => {handleClick(4)}}><Text>LEFT</Text></TouchableOpacity>
        </View>
        <View style={styles.col}>
          <TouchableOpacity style={styles.button} onPress={() => {handleClick(5)}}><Text>STOP</Text></TouchableOpacity>
        </View>
        <View style={styles.col}>
          <TouchableOpacity style={styles.button} onPress={() => {handleClick(3)}}><Text>RIGHT</Text></TouchableOpacity>
        </View>
      </View>
      <View style={styles.row}>
        <View style={styles.col}>
        </View>
        <View style={styles.col}>
          <TouchableOpacity style={styles.button} onPress={() => {handleClick(2)}}><Text>BACK</Text></TouchableOpacity>
        </View>
        <View style={styles.col}>
        </View>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: "column",
    padding: 10,
    backgroundColor: "#FEFEFE"
  },
  
  row: {
    flex: 1,
    flexDirection: "row"
  },

  col: {
    flex: 1,
    justifyContent: "space-evenly",
    padding: 2
  },

  button: {
    alignItems: "center",
    backgroundColor: "#888888",
    padding: 10
  }
});

export default App;
