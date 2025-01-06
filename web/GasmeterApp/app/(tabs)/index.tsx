import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Dimensions } from 'react-native';
import axios from 'axios';
import { useRouter } from "expo-router";

// Gas-Konfiguration mit Namen, Einheiten und Schwellenwerten
const gasConfig = {
  CO2: { name: 'Kohlenstoffdioxid', unit: 'ppm', maxValue: 5000, warningThreshold: 1000, criticalThreshold: 2000, dangerThreshold: 5000 },
  CH4: { name: 'Methan', unit: 'ppm', maxValue: 50000, warningThreshold: 5000, criticalThreshold: 10000, dangerThreshold: 50000 },
  O2: { name: 'Sauerstoff', unit: '%', maxValue: 100, warningThreshold: 19.5, criticalThreshold: 21, dangerThreshold: 23 },
  CO: { name: 'Kohlenmonoxid', unit: 'ppm', maxValue: 1000, warningThreshold: 35, criticalThreshold: 200, dangerThreshold: 1000 },
};

// Typdefinitionen für Gase und Sensordaten
type GasType = 'CO2' | 'CH4' | 'O2' | 'CO';

type Sensor = {
  type: GasType; // Gasbezeichnung (z. B. CO2, CH4, etc.)
  val: number;   // Wert des Sensors
};

type LatestData = {
  device_id: string; // Geräte-ID
  system_datetime: string; // Zeitstempel der Daten
  pct: number;       // Batteriestatus in Prozent
  sensors: Sensor[]; // Liste der Sensorsignale
};

const HomeScreen = () => {
  const [flippedBoxes, setFlippedBoxes] = useState<{ [key: string]: boolean }>({});
  const [latestData, setLatestData] = useState<LatestData | null>(null);
  const router = useRouter(); // Router-Instanz

  // Abrufen der aktuellen Sensordaten vom Server
  const fetchData = () => {
    axios
      .get('http://stuttgart-int.de/get-last-entity') // Abruf der aktuellen Daten
      .then(response => {
        setLatestData(response.data); // Aktualisiere die neuesten Daten
      })
      .catch(error => {
        console.error('Error fetching data:', error); // Fehlerbehandlung
      });
  };

  // Lädt initial die Sensordaten und setzt ein Intervall für regelmäßige Updates
  useEffect(() => {
    fetchData(); // Initialer Datenabruf
    const interval = setInterval(fetchData, 1000); // Aktualisierung alle 1 Sekunde
    return () => clearInterval(interval); // Aufräumen des Intervalls
  }, []);

  // Funktion zum Umschalten des Flip-Zustands
  const toggleFlip = (gas: string) => {
    setFlippedBoxes(prevState => ({
      ...prevState,
      [gas]: !prevState[gas], // Zustand direkt umschalten
    }));
  };

  // Darstellung eines Balkendiagramms mit Schwellenwerten
  const renderGradientBarWithArrow = (
    value: number,
    maxValue: number,
    warningThreshold: number,
    criticalThreshold: number,
    dangerThreshold: number
  ) => {
    const percentage = Math.min((value / maxValue) * 100, 100);
    const warningPercentage = (warningThreshold / maxValue) * 100;
    const criticalPercentage = (criticalThreshold / maxValue) * 100;
    const dangerPercentage = (dangerThreshold / maxValue) * 100;

    return (
      <View style={styles.barContainer}>
        <View style={[styles.barSegment, { width: `${warningPercentage}%`, backgroundColor: 'rgba(0, 128, 0, 0.6)' }]} />
        <View style={[styles.barSegment, { width: `${criticalPercentage - warningPercentage}%`, backgroundColor: 'rgba(255, 255, 0, 0.6)' }]} />
        <View style={[styles.barSegment, { width: `${dangerPercentage - criticalPercentage}%`, backgroundColor: 'rgba(255, 0, 0, 0.6)' }]} />
        <View style={[styles.barSegmentRed, { width: `${100 - dangerPercentage}%`, backgroundColor: 'rgba(139, 0, 0, 0.6)' }]} />
        <View style={[styles.arrow, { left: `${percentage}%` }]} />
      </View>
    );
  };

  // Darstellung einer Sensor-Box für ein spezifisches Gas
  const renderGasBox = (sensor: Sensor) => {
    const { type, val } = sensor;
    const config = gasConfig[type];
    if (!config) return null;

    const { name, unit, maxValue, warningThreshold, criticalThreshold, dangerThreshold } = config;

    return (
      <View style={styles.gasBox} key={type}>
        {flippedBoxes[type] ? (
          <View style={styles.infoBack}>
            <Text style={styles.infoText}>
              {`Information about ${name}: This gas is measured in ${unit}.\nIt has a danger threshold of ${dangerThreshold}.`}
            </Text>
          </View>
        ) : (
          <View style={styles.boxContent}>
            <Text style={styles.gasTitle}>{name}</Text>
            <Text style={styles.gasSymbol}>{type}</Text>
            <Text style={styles.gasValue}>{`${val} ${unit}`}</Text>
            {renderGradientBarWithArrow(val, maxValue, warningThreshold, criticalThreshold, dangerThreshold)}
          </View>
        )}
        <TouchableOpacity
          style={styles.infoButton}
          onPress={() => toggleFlip(type)}
        >
          <Text style={styles.infoButtonText}>{'\u24D8'}</Text>
        </TouchableOpacity>
      </View>
    );
  };

  // Haupt-Render-Funktion
  return (
    <View style={styles.container}>
      <Text style={styles.topRightText}>Device Battery: {latestData?.pct}%</Text>
      <Text style={styles.header}>Gasmeter</Text>
      <Text style={styles.dataHeader}>Device ID: {latestData?.device_id}</Text>
      
      <View style={styles.gasContainer}>
        {latestData?.sensors.map(sensor => renderGasBox(sensor))}
      </View>

      <Text style={styles.title}>Latest Sensor Data</Text>
      {latestData ? (
        <View style={styles.dataContainer}>
          <Text>
            Timestamp: {latestData?.system_datetime ? new Date(latestData.system_datetime).toLocaleTimeString() : 'Loading...'}
          </Text>
        </View>
      ) : (
        <Text>Loading...</Text>
      )}

      {/* Button zur Navigation zur zweiten Seite */}
      <TouchableOpacity style={styles.navigationButton} onPress={() => router.push('/secondPage')}>
        <Text style={styles.navigationButtonText}>Historical Data</Text>
      </TouchableOpacity>
    </View>
  );
};

export default HomeScreen;


  const colors = {
    primary: '#4CAF50',
    secondary: '#8BC34A',
    warning: '#FFEB3B',
    danger: '#F44336',
    textDark: '#212121',
    textLight: '#757575',
    background: '#E0E0E0',
    gasBackground: '#ffffff',
  };


  const styles = StyleSheet.create({
    gasBox: {
      backgroundColor: colors.gasBackground,
      padding: 20,
      borderRadius: 15,
      width: Dimensions.get('window').width * 0.40,
      minHeight: 200,
      marginBottom: 20,
      alignItems: 'center',
      justifyContent: 'center',
      elevation: 5,
      shadowColor: '#000',
      shadowOffset: {
        width: 0,
        height: 4,
      },
      shadowOpacity: 0.2,
      shadowRadius: 6,
      borderWidth: 1,
      borderColor: '#E0E0E0',
      backfaceVisibility: 'hidden', // Wichtig für die Animation
    },  
    container: {
      flex: 1,
      backgroundColor: colors.background,
      alignItems: 'center',
      justifyContent: 'center',
      padding: 20,
    },
    headerContainer: {
      flexDirection: 'row',
      justifyContent: 'space-between',
      alignItems: 'center',
      backgroundColor: 'black',
      padding: 10,
    },
    headerText: {
      color: '#fff',
      fontSize: 18,
      fontWeight: 'bold',
    },
    header: {
      fontSize: 36,
      fontWeight: 'bold',
      marginBottom: 15,
      marginTop: 5,
      color: colors.textDark,
      textAlign: 'center',
    },
    dataHeader: {
      fontSize: 25,
      fontWeight: 'bold',
      marginBottom: 50,
      color: colors.textDark,
      textAlign: 'center',
    },
    topLeftText: {
      position: 'absolute',
      top: 10, // Abstand von der oberen Kante
      left: 10, // Abstand von der linken Kante
      fontSize: 16,
      fontWeight: 'bold',
      color: 'black',
    },
    topRightText: {
      position: 'absolute',
      top: 10, // Abstand von der oberen Kante
      right: 10, // Abstand von der linken Kante
      fontSize: 16,
      fontWeight: 'bold',
      color: 'black',
    },
    boxContent: {
      position: 'absolute',
      width: '100%',
      height: '100%',
      backfaceVisibility: 'hidden',
      alignItems: 'center',
      justifyContent: 'center',
    },
    gasContainer: {
      flexDirection: 'row',
      flexWrap: 'wrap',
      justifyContent: 'space-around',
      paddingHorizontal: 10,
    },
    gasTitle: {
      fontSize: 20,
      fontWeight: '600',
      color: colors.textDark,
      marginBottom: 5,
      textAlign: 'center',
    },
    gasSymbol: {
      fontSize: 18,
      fontWeight: '500',
      color: colors.textDark,
      marginBottom: 10,
      textAlign: 'center',
    },
    gasValue: {
      fontSize: 18,
      fontWeight: '400',
      color: colors.textLight,
      marginBottom: 15,
      textAlign: 'center',
    },
    barContainer: {
      position: 'relative',
      width: '80%',
      height: 10,
      flexDirection: 'row',
      borderRadius: 10,
      overflow: 'visible',
    },
    barSegment: {
      height: '100%',
      opacity: 0.6,
    },
    barSegmentRed: {
      height: '100%',
      opacity: 0.6,
      borderTopRightRadius: 10,
      borderBottomRightRadius: 10,
    },
    arrowContainer: {
      position: 'absolute',
      top: -8,
      zIndex: 0,
      transform: [{ translateX: -10 }],
    },
    arrow: {
      position: 'absolute',
      top: -5, // Passe die Höhe so an, dass der Pfeil korrekt sitzt
      width: 0,
      height: 0,
      borderLeftWidth: 6,
      borderRightWidth: 6,
      borderBottomWidth: 14,
      borderLeftColor: 'transparent',
      borderRightColor: 'transparent',
      borderBottomColor: 'blue',
      transform: [{ rotate: '180deg' }],
    },    
    infoButton: {
      position: 'absolute',
      top: 6,
      right: 6,
      width: 30,
      height: 30,
      borderRadius: 15,
      backgroundColor: '#fff',
      justifyContent: 'center',
      alignItems: 'center',
      zIndex: 10, // Bringt den Button in den Vordergrund
    },    
    infoButtonText: {
      color: '#000',
      fontSize: 16,
      fontWeight: 'bold',
    },
    infoBack: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
      backgroundColor: '#fff',
      borderRadius: 10,
      backfaceVisibility: 'hidden', // Rückseite der Box
    },
    infoText: {
      color: colors.textDark,
      fontSize: 14,
      textAlign: 'center',
      flexWrap: 'wrap',
    },
    title: {
      fontSize: 22,
      fontWeight: 'bold',
    },
    dataContainer: {
      marginTop: 20,
    },
    modalContainer: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
      padding: 20,
    },
    modalTitle: {
      fontSize: 20,
      fontWeight: 'bold',
      marginBottom: 10,
    },
    chart: {
      marginVertical: 8,
      borderRadius: 16,
    },
    topLeftButton: {
      position: 'absolute',
      top: 10,
      left: 10,
      backgroundColor: '#4CAF50',
      padding: 10,
      borderRadius: 5,
    },
    buttonText: {
      color: '#fff',
      fontWeight: 'bold',
    },
    navigationButton: {
      marginTop: 20, // Abstand nach oben
      backgroundColor: '#4CAF50', // Grüne Farbe
      padding: 10, // Padding
      borderRadius: 5, // Abgerundete Ecken
      alignItems: 'center', // Zentrierter Text
    },
    navigationButtonText: {
      color: '#fff', // Weißer Text
      fontWeight: 'bold', // Fettschrift
    },
});
