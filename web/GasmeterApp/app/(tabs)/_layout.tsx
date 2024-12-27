import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Animated } from 'react-native';
import axios from 'axios';

const HomeScreen = () => {
  const [flippedBoxes, setFlippedBoxes] = useState<{ [key: string]: boolean }>({});
  const [latestData, setLatestData] = useState<any>(null);

  const flipAnimations: { [key: string]: Animated.Value } = {
    Methan: new Animated.Value(0),
    LPG: new Animated.Value(0),
    CO: new Animated.Value(0),
    H2: new Animated.Value(0),
  };

  useEffect(() => {
    axios.get('http://127.0.0.1:5000/get-last-entity')
      .then(response => {
        setLatestData(response.data);
      })
      .catch(error => {
        console.error('There was an error fetching the data!', error);
      });
  }, []);

  const toggleFlip = (gas: string) => {
    const isFlipped = flippedBoxes[gas];

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
            { width: `${warningPercentage}%`, backgroundColor: 'rgba(0, 128, 0, 0.6)', borderTopLeftRadius: 10, borderBottomLeftRadius: 10 },
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
            { width: `${100 - dangerPercentage}%`, backgroundColor: 'rgba(139, 0, 0, 0.6)',},
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

  const renderGasBox = (
    gas: string,
    title: string,
    symbol: string,
    value: string,
    barProps: [number, number, number, number, number],
    infoText: string
  ) => {
    const flipStyle = {
      transform: [
        {
          rotateY: flipAnimations[gas].interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '180deg'],
          }),
        },
      ],
    };

    return (
      <Animated.View style={[styles.gasBox, flipStyle]} key={gas}>
        {flippedBoxes[gas] ? (
          <View style={styles.infoBack}>
            <Text style={styles.infoText}>{infoText}</Text>
          </View>
        ) : (
          <>
            <Text style={styles.gasTitle}>{title}</Text>
            <Text style={styles.gasSymbol}>{symbol}</Text>
            <Text style={styles.gasValue}>{value}</Text>
            {renderGradientBarWithArrow(...barProps)}
          </>
        )}
        <TouchableOpacity
          style={styles.infoButton}
          onPress={() => toggleFlip(gas)}
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
        {renderGasBox(
          'Methan',
          'Methan',
          'CH₄',
          '30 ppm',
          [30, 100, 40, 50, 100],
          'Grenzwert: Über 5% (50,000 ppm) in der Luft bildet Methan eine explosive Mischung.'
        )}
        {renderGasBox(
          'LPG',
          'Flüssiggas',
          'LPG',
          '10 ppm',
          [10, 50, 10, 20, 50],
          'Grenzwert: Über 2% (20,000 ppm) in der Luft ist LPG explosiv; hohe Konzentrationen sind gesundheitsschädlich.'
        )}
        {renderGasBox(
          'CO',
          'Kohlenmonoxid',
          'CO',
          '200 ppm',
          [200, 1000, 200, 800, 1000],
          'Grenzwert: Über 35 ppm ist CO gefährlich; ab 200 ppm kann es zu Vergiftungen führen.'
        )}
        {renderGasBox(
          'H2',
          'Wasserstoff',
          'H₂',
          '40 ppm',
          [40, 80, 30, 40, 80],
          'Grenzwert: Über 4% (40,000 ppm) Wasserstoff in der Luft ist explosionsgefährlich.'
        )}
      </View>
      <Text style={styles.title}>Latest Sensor Data</Text>
      {latestData ? (
        <View style={styles.dataContainer}>
          <Text>Device ID: {latestData.device_id}</Text>
          <Text>Timestamp: {latestData.timestamp}</Text>
          <Text>Battery Percentage: {latestData.battery_pct}</Text>
          <Text>Battery Charging: {latestData.battery_chg ? 'Yes' : 'No'}</Text>
          <Text>Sensors: {JSON.stringify(latestData.sensors)}</Text>
          <Text>Temperature: {JSON.stringify(latestData.temperature)}</Text>
          <Text>Status: {JSON.stringify(latestData.status)}</Text>
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
