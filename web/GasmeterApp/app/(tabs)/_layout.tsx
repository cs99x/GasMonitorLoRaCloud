import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Animated } from 'react-native';
import axios from 'axios';

const HomeScreen = () => {
  const [flippedBoxes, setFlippedBoxes] = useState<{ [key: string]: boolean }>({});
  const [latestData, setLatestData] = useState<LatestData | null>(null);

  const flipAnimations: { [key: string]: Animated.Value } = {};

  const MOCK_API_URL = 'https://c58e75e3-e1d4-4d97-884d-172b80a6f4ca.mock.pstmn.io/api/get-last-entity';

   // Funktion zum Abrufen der Daten
   const fetchData = () => {
    axios.get(MOCK_API_URL)
      .then(response => {
        console.log('Updated data:', response.data);
        setLatestData(response.data);
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  };

  useEffect(() => {
    const fetchData = () => {
      axios.get(MOCK_API_URL)
        .then(response => {
          console.log('Updated data:', response.data);
          setLatestData(response.data);
        })
        .catch(error => {
          console.error('Error fetching data:', error);
        });
    };
  
    // Initialer Datenabruf
    fetchData();
  
    // Aktualisierung alle 10 Sekunden
    const interval = setInterval(fetchData, 1000);
  
    // Bereinigung bei Komponenten-Demontage
    return () => clearInterval(interval);
  }, []);
    
  /*useEffect(() => {
    axios.get('http://193.197.230.228:80/get-last-entity')
      .then(response => {
        console.log('Received data:', response.data);
        setLatestData(response.data);
        initializeAnimations(response.data.sensors);
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }, []); */

  type Sensor = {
    name: string;
    type: string;
    unit: string;
    val: number;
  };  

  type LatestData = {
    device_id: string;
    timestamp: string;
    battery_pct: number;
    battery_chg: boolean;
    sensors: Sensor[];
    status: {
      alarm: boolean;
      fault: boolean;
    };
    temperature: {
      type: string;
      unit: string;
      val: number;
    };
  };  
  
  const initializeAnimations = (sensors: Sensor[]) => {
    sensors.forEach(sensor => {
      flipAnimations[sensor.name] = new Animated.Value(0);
    });
  };
  

  const toggleFlip = (gas: string) => {
    const isFlipped = flippedBoxes[gas]; // `flippedBoxes` ist jetzt richtig typisiert
    Animated.timing(flipAnimations[gas], {
      toValue: isFlipped ? 0 : 1,
      duration: 500,
      useNativeDriver: true,
    }).start();
  
    setFlippedBoxes((prevState) => ({
      ...prevState,
      [gas]: !isFlipped,
    }));
  };  

  const calculateBarProps = (type: string, value: number): [number, number, number, number, number] => {
    switch (type) {
      case 'CH4': // Methane
        return [value, 50000, 5000, 10000, 50000];
      case 'CO': // Carbon Monoxide
        return [value, 1000, 35, 200, 1000];
      case 'O2': // Oxygen
        return [value, 100, 19.5, 21, 23];
      case 'CO2': // Carbon Dioxide
        return [value, 5000, 1000, 2000, 5000];
      default:
        return [value, 100, 50, 75, 100]; // Default thresholds
    }
  };  

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
        <View
          style={[
            styles.barSegment,
            {
              width: `${warningPercentage}%`,
              backgroundColor: 'rgba(0, 128, 0, 0.6)',
              borderTopLeftRadius: 10,
              borderBottomLeftRadius: 10,
            },
          ]}
        />
        <View
          style={[
            styles.barSegment,
            {
              width: `${criticalPercentage - warningPercentage}%`,
              backgroundColor: 'rgba(255, 255, 0, 0.6)',
            },
          ]}
        />
        <View
          style={[
            styles.barSegment,
            {
              width: `${dangerPercentage - criticalPercentage}%`,
              backgroundColor: 'rgba(255, 0, 0, 0.6)',
            },
          ]}
        />
        <View
          style={[
            styles.barSegmentRed,
            {
              width: `${100 - dangerPercentage}%`,
              backgroundColor: 'rgba(139, 0, 0, 0.6)',
            },
          ]}
        />
        <View
          style={[
            styles.arrowContainer,
            { left: `${percentage}%` },
          ]}
        >
          <View style={styles.arrowShape} />
        </View>
      </View>
    );
  };  

  const renderGasBox = (sensor: Sensor) => {
    const { name, type, val, unit } = sensor;
    const flipStyle = {
      transform: [
        {
          rotateY: flipAnimations[name]?.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '180deg'],
          }),
        },
      ],
    };
  
    return (
      <Animated.View style={[styles.gasBox, flipStyle]} key={name}>
        {flippedBoxes[name] ? (
          <View style={styles.infoBack}>
            <Text style={styles.infoText}>Information about {name}</Text>
          </View>
        ) : (
          <>
            <Text style={styles.gasTitle}>{name}</Text>
            <Text style={styles.gasSymbol}>{type}</Text>
            <Text style={styles.gasValue}>{`${val} ${unit}`}</Text>
            {renderGradientBarWithArrow(...calculateBarProps(type, val))}
          </>
        )}
        <TouchableOpacity
          style={styles.infoButton}
          onPress={() => toggleFlip(name)}
        >
          <Text style={styles.infoButtonText}>{'\u24D8'}</Text>
        </TouchableOpacity>
      </Animated.View>
    );
  };  

  return (
    <View style={styles.container}>
    <Text style={styles.header}>Gasmeter</Text>
    <View style={styles.gasContainer}>
    {latestData && latestData.sensors.map(renderGasBox)}
    </View>
    <Text style={styles.title}>Latest Sensor Data</Text>
    {latestData ? (
    <View style={styles.dataContainer}>
      <Text>Device ID: {latestData.device_id}</Text>
      <Text>Timestamp: {new Date(latestData.timestamp).toLocaleString()}</Text>
      <Text>Battery Percentage: {latestData.battery_pct}%</Text>
      <Text>Battery Charging: {latestData.battery_chg ? 'Yes' : 'No'}</Text>
      <Text>Status:</Text>
      <Text> - Alarm: {latestData.status.alarm ? 'Yes' : 'No'}</Text>
      <Text> - Fault: {latestData.status.fault ? 'Yes' : 'No'}</Text>
      <Text>
        Temperature: {latestData.temperature.val} {latestData.temperature.unit}
      </Text>
      </View>
    ) : (
      <Text>Loading...</Text>
    )}
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
  container: {
    flex: 1,
    backgroundColor: colors.background,
    alignItems: 'center',
    justifyContent: 'center',
    padding: 20,
  },
  header: {
    fontSize: 36,
    fontWeight: 'bold',
    marginBottom: 60,
    color: colors.textDark,
    textAlign: 'center',
  },
  gasContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-around',
    paddingHorizontal: 10,
  },
  gasBox: {
    backgroundColor: colors.gasBackground,
    padding: 20,
    borderRadius: 15,
    width: '45%',
    height: 180,
    marginBottom: 20,
    alignItems: 'center',
    justifyContent: 'center',
    elevation: 5,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 4 },
    shadowOpacity: 0.2,
    shadowRadius: 6,
    borderWidth: 1,
    borderColor: '#E0E0E0',
    backfaceVisibility: 'hidden',
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
    width: '100%',
    height: 10,
    flexDirection: 'row',
    borderRadius: 10,
    overflow: 'visible', // Ensure the arrow remains visible
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
  arrowShape: {
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
    width: 23,
    height: 23,
    borderRadius: 5,
    backgroundColor: '#fff',
    justifyContent: 'center',
    alignItems: 'center',
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
    padding: 10,
  },
  infoText: {
    color: colors.textDark,
    fontSize: 14,
    textAlign: 'center',
    flexWrap: 'wrap',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 20,
  },
  dataContainer: {
    marginTop: 20,
  },
});
