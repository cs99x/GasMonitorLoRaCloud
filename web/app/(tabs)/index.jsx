import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

const HomeScreen = () => {
  return (
    <View style={styles.container}>
      {/* Überschrift */}
      <Text style={styles.header}>Gasmeter</Text>

      {/* Gas Anzeigen */}
      <View style={styles.gasContainer}>
        <View style={styles.gasBox}>
          <Text style={styles.gasTitle}>CO₂</Text>
          <Text style={styles.gasValue}>400 ppm</Text>
        </View>

        <View style={styles.gasBox}>
          <Text style={styles.gasTitle}>O₂</Text>
          <Text style={styles.gasValue}>21%</Text>
        </View>

        <View style={styles.gasBox}>
          <Text style={styles.gasTitle}>CH₄</Text>
          <Text style={styles.gasValue}>5 ppm</Text>
        </View>

        <View style={styles.gasBox}>
          <Text style={styles.gasTitle}>CO</Text>
          <Text style={styles.gasValue}>0 ppm</Text>
        </View>
      </View>
    </View>
  );
}

export default HomeScreen;

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5',
    alignItems: 'center',
    justifyContent: 'center',
    padding: 20,
  },
  header: {
    fontSize: 32,
    fontWeight: 'bold',
    marginBottom: 30,
    color: '#333',
  },
  gasContainer: {
    flexDirection: 'row', // Horizontale Verteilung
    flexWrap: 'wrap', // Automatischer Umbruch bei Platzmangel
    justifyContent: 'space-around',
    width: '100%',
  },
  gasBox: {
    backgroundColor: '#ffffff',
    padding: 20,
    borderRadius: 10,
    width: '45%', // Zwei Boxen pro Zeile
    marginBottom: 20,
    alignItems: 'center',
    elevation: 3, // Schatten für Android
    shadowColor: '#000', // Schatten für iOS
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 3,
  },
  gasTitle: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#444',
  },
  gasValue: {
    fontSize: 20,
    color: '#888',
    marginTop: 10,
  },
});
