import React, { useState } from 'react';
import {
  SafeAreaView,
  StyleSheet,
  Text,
  View,
  Pressable,
  ScrollView,
} from 'react-native';

type StationStatus = 'available' | 'occupied' | 'maintenance';

type Station = {
  id: number;
  name: string;
  status: StationStatus;
};

const initialStations: Station[] = [
  {
    id: 1,
    name: 'Polia 01',
    status: 'available',
  },
  {
    id: 2,
    name: 'Polia 02',
    status: 'occupied',
  },
  {
    id: 3,
    name: 'Polia 03',
    status: 'maintenance',
  },
];

export default function App() {
  const [stations, setStations] = useState<Station[]>(initialStations);

  function toggleMaintenance(stationId: number) {
    setStations((currentStations) =>
      currentStations.map((station) => {
        if (station.id !== stationId) {
          return station;
        }

        return {
          ...station,
          status:
            station.status === 'maintenance'
              ? 'available'
              : 'maintenance',
        };
      }),
    );
  }

  function getStatusText(status: StationStatus) {
    switch (status) {
      case 'available':
        return 'Disponível';

      case 'occupied':
        return 'Ocupada';

      case 'maintenance':
        return 'Manutenção';
    }
  }

  function getStatusColor(status: StationStatus) {
    switch (status) {
      case 'available':
        return '#16a34a';

      case 'occupied':
        return '#dc2626';

      case 'maintenance':
        return '#2563eb';
    }
  }

  const availableStations = stations.filter(
    (station) => station.status === 'available',
  ).length;

  const occupiedStations = stations.filter(
    (station) => station.status === 'occupied',
  ).length;

  const maintenanceStations = stations.filter(
    (station) => station.status === 'maintenance',
  ).length;

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView
        contentContainerStyle={styles.content}
        showsVerticalScrollIndicator={false}
      >
        {/* Cabeçalho */}
        <View style={styles.header}>
          <Text style={styles.title}>Gym Pole Monitor</Text>

          <Text style={styles.subtitle}>
            Painel de monitoramento
          </Text>

          <View style={styles.employeeBadge}>
            <Text style={styles.employeeText}>
              Funcionário
            </Text>
          </View>
        </View>

        {/* Resumo */}
        <Text style={styles.sectionTitle}>
          Visão geral
        </Text>

        <View style={styles.summaryContainer}>
          <View style={styles.summaryCard}>
            <View
              style={[
                styles.summaryIndicator,
                { backgroundColor: '#16a34a' },
              ]}
            />

            <Text style={styles.summaryNumber}>
              {availableStations}
            </Text>

            <Text style={styles.summaryLabel}>
              Disponíveis
            </Text>
          </View>

          <View style={styles.summaryCard}>
            <View
              style={[
                styles.summaryIndicator,
                { backgroundColor: '#dc2626' },
              ]}
            />

            <Text style={styles.summaryNumber}>
              {occupiedStations}
            </Text>

            <Text style={styles.summaryLabel}>
              Ocupadas
            </Text>
          </View>

          <View style={styles.summaryCard}>
            <View
              style={[
                styles.summaryIndicator,
                { backgroundColor: '#2563eb' },
              ]}
            />

            <Text style={styles.summaryNumber}>
              {maintenanceStations}
            </Text>

            <Text style={styles.summaryLabel}>
              Manutenção
            </Text>
          </View>
        </View>

        {/* Estações */}
        <Text style={styles.sectionTitle}>
          Estações
        </Text>

        {stations.map((station) => (
          <View key={station.id} style={styles.stationCard}>
            <View style={styles.cardHeader}>
              <View>
                <Text style={styles.stationName}>
                  {station.name}
                </Text>

                <Text style={styles.stationDescription}>
                  Estação de polia
                </Text>
              </View>

              <View
                style={[
                  styles.statusIndicator,
                  {
                    backgroundColor: getStatusColor(
                      station.status,
                    ),
                  },
                ]}
              />
            </View>

            <Text
              style={[
                styles.statusText,
                {
                  color: getStatusColor(station.status),
                },
              ]}
            >
              {getStatusText(station.status)}
            </Text>

            {station.status !== 'occupied' && (
              <Pressable
                style={[
                  styles.button,
                  station.status === 'maintenance' &&
                    styles.releaseButton,
                ]}
                onPress={() =>
                  toggleMaintenance(station.id)
                }
              >
                <Text style={styles.buttonText}>
                  {station.status === 'maintenance'
                    ? 'Liberar estação'
                    : 'Colocar em manutenção'}
                </Text>
              </Pressable>
            )}

            {station.status === 'occupied' && (
              <View style={styles.occupiedInfo}>
                <Text style={styles.occupiedInfoText}>
                  Ocupação detectada pelo sensor
                </Text>
              </View>
            )}
          </View>
        ))}
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f3f4f6',
  },

  content: {
    padding: 20,
    maxWidth: 900,
    width: '100%',
    alignSelf: 'center',
  },

  header: {
    marginBottom: 28,
  },

  title: {
    fontSize: 30,
    fontWeight: '700',
    color: '#111827',
  },

  subtitle: {
    marginTop: 5,
    fontSize: 16,
    color: '#6b7280',
  },

  employeeBadge: {
    alignSelf: 'flex-start',
    marginTop: 12,
    backgroundColor: '#e5e7eb',
    paddingHorizontal: 12,
    paddingVertical: 6,
    borderRadius: 20,
  },

  employeeText: {
    fontSize: 13,
    fontWeight: '600',
    color: '#374151',
  },

  sectionTitle: {
    fontSize: 21,
    fontWeight: '700',
    marginBottom: 14,
    color: '#111827',
  },

  summaryContainer: {
    flexDirection: 'row',
    gap: 10,
    marginBottom: 28,
  },

  summaryCard: {
    flex: 1,
    backgroundColor: '#ffffff',
    borderRadius: 12,
    padding: 16,
    minHeight: 115,
    justifyContent: 'center',
  },

  summaryIndicator: {
    width: 10,
    height: 10,
    borderRadius: 5,
    marginBottom: 8,
  },

  summaryNumber: {
    fontSize: 28,
    fontWeight: '700',
    color: '#111827',
  },

  summaryLabel: {
    marginTop: 2,
    fontSize: 13,
    color: '#6b7280',
  },

  stationCard: {
    backgroundColor: '#ffffff',
    borderRadius: 12,
    padding: 18,
    marginBottom: 15,
    elevation: 3,
  },

  cardHeader: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
  },

  stationName: {
    fontSize: 20,
    fontWeight: '600',
    color: '#111827',
  },

  stationDescription: {
    marginTop: 3,
    fontSize: 13,
    color: '#9ca3af',
  },

  statusIndicator: {
    width: 15,
    height: 15,
    borderRadius: 8,
  },

  statusText: {
    marginTop: 12,
    fontSize: 16,
    fontWeight: '600',
  },

  button: {
    marginTop: 18,
    backgroundColor: '#2563eb',
    paddingVertical: 12,
    borderRadius: 8,
    alignItems: 'center',
  },

  releaseButton: {
    backgroundColor: '#16a34a',
  },

  buttonText: {
    color: '#ffffff',
    fontSize: 15,
    fontWeight: '600',
  },

  occupiedInfo: {
    marginTop: 18,
    backgroundColor: '#fef2f2',
    paddingVertical: 10,
    paddingHorizontal: 12,
    borderRadius: 8,
  },

  occupiedInfoText: {
    color: '#991b1b',
    fontSize: 13,
    fontWeight: '500',
  },
});