/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Sidharth Nabar <snabar@uw.edu>, He Wu <mdzz@u.washington.edu>
 */

#include "basic-energy-source.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BasicEnergySource");

NS_OBJECT_ENSURE_REGISTERED (BasicEnergySource);

TypeId
BasicEnergySource::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BasicEnergySource")
    .SetParent<EnergySource> ()
    .SetGroupName ("Energy")
    .AddConstructor<BasicEnergySource> ()
    .AddAttribute ("BasicEnergySourceInitialEnergyJ",
                   "Initial energy stored in basic energy source.",
                   DoubleValue (10),  // in Joules
                   MakeDoubleAccessor (&BasicEnergySource::SetInitialEnergy,
                                       &BasicEnergySource::GetInitialEnergy),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("BasicEnergySupplyVoltageV",
                   "Initial supply voltage for basic energy source.",
                   DoubleValue (3.0), // in Volts
                   MakeDoubleAccessor (&BasicEnergySource::SetSupplyVoltage,
                                       &BasicEnergySource::GetSupplyVoltage),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("BasicEnergyLowBatteryThreshold",
                   "Low battery threshold for basic energy source.",
                   DoubleValue (0.10), // as a fraction of the initial energy
                   MakeDoubleAccessor (&BasicEnergySource::m_lowBatteryTh),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("BasicEnergyHighBatteryThreshold",
                   "High battery threshold for basic energy source.",
                   DoubleValue (0.15), // as a fraction of the initial energy
                   MakeDoubleAccessor (&BasicEnergySource::m_highBatteryTh),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PeriodicEnergyUpdateInterval",
                   "Time between two consecutive periodic energy updates.",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&BasicEnergySource::SetEnergyUpdateInterval,
                                     &BasicEnergySource::GetEnergyUpdateInterval),
                   MakeTimeChecker ())
    .AddTraceSource ("RemainingEnergy",
                     "Remaining energy at BasicEnergySource.",
                     MakeTraceSourceAccessor (&BasicEnergySource::m_remainingEnergyJ),
                     "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

BasicEnergySource::BasicEnergySource ()
{
  NS_LOG_FUNCTION (this);
  m_lastUpdateTime = Seconds (0.0);
  m_depleted = false;
}

BasicEnergySource::~BasicEnergySource ()
{
  NS_LOG_FUNCTION (this);
}

void
BasicEnergySource::SetInitialEnergy (double initialEnergyJ)
{
  NS_LOG_FUNCTION (this << initialEnergyJ);
  NS_ASSERT (initialEnergyJ >= 0);
  m_initialEnergyJ = initialEnergyJ;
  m_remainingEnergyJ = m_initialEnergyJ;
}

void
BasicEnergySource::SetSupplyVoltage (double supplyVoltageV)
{
  NS_LOG_FUNCTION (this << supplyVoltageV);
  m_supplyVoltageV = supplyVoltageV;
}

void
BasicEnergySource::SetEnergyUpdateInterval (Time interval)
{
  NS_LOG_FUNCTION (this << interval);
  m_energyUpdateInterval = interval;
}

Time
BasicEnergySource::GetEnergyUpdateInterval (void) const
{
  NS_LOG_FUNCTION (this);
  return m_energyUpdateInterval;
}

double
BasicEnergySource::GetSupplyVoltage (void) const
{
  NS_LOG_FUNCTION (this);
  return m_supplyVoltageV;
}

double
BasicEnergySource::GetInitialEnergy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_initialEnergyJ;
}

double
BasicEnergySource::GetRemainingEnergy (void)
{
  NS_LOG_FUNCTION (this);
  // update energy source to get the latest remaining energy.
  UpdateEnergySource ();
  return m_remainingEnergyJ;
}

double
BasicEnergySource::GetEnergyFraction (void)
{
  NS_LOG_FUNCTION (this);
  // update energy source to get the latest remaining energy.
  UpdateEnergySource ();
  return m_remainingEnergyJ / m_initialEnergyJ;
}

void
BasicEnergySource::UpdateEnergySource (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("BasicEnergySource:Updating remaining energy.");

  double remainingEnergy = m_remainingEnergyJ;
  CalculateRemainingEnergy ();

  m_lastUpdateTime = Simulator::Now ();

  if (!m_depleted && m_remainingEnergyJ <= m_lowBatteryTh * m_initialEnergyJ)
    {
      m_depleted = true;
      HandleEnergyDrainedEvent ();
    }
  else if (m_depleted && m_remainingEnergyJ > m_highBatteryTh * m_initialEnergyJ)
    {
      m_depleted = false;
      HandleEnergyRechargedEvent ();
    }
  else if (m_remainingEnergyJ != remainingEnergy)
    {
      NotifyEnergyChanged ();
    }

  if (m_energyUpdateEvent.IsExpired ())
    {
      m_energyUpdateEvent = Simulator::Schedule (m_energyUpdateInterval,
                                                 &BasicEnergySource::UpdateEnergySource,
                                                 this);
    }
}


//Função de recarga
void
BasicEnergySource::Recharge (void)
{
        //m_initialEnergyJ
        //m_remainingEnergyJ
        NS_LOG_UNCOND("RECARREGANDO UAV");
        if(!recharged){
                if(m_remainingEnergyJ + (m_initialEnergyJ*0.2) < m_initialEnergyJ){
                        
                        //NS_LOG_UNCOND("ENERGIA ATUAL:");
                        //NS_LOG_UNCOND(m_remainingEnergyJ);
                        m_remainingEnergyJ = m_remainingEnergyJ + (m_initialEnergyJ*0.2);
                        Simulator::Schedule (Seconds(1),
                                                 &BasicEnergySource::Recharge,
                                                 this);
                        //NS_LOG_UNCOND("ENERGIA PARCIALMENTE CARREGADA");
                        //NS_LOG_UNCOND(m_remainingEnergyJ);
                }else{
                        m_remainingEnergyJ = m_initialEnergyJ;
                        recharged = true;
                        //NS_LOG_UNCOND("UAV COMPLETAMENTE CARREGADO");
                        //NS_LOG_UNCOND(m_remainingEnergyJ);
                }
        }

        
}

void
BasicEnergySource::CallRecharge (void){

        recharged = false;
        //NS_LOG_UNCOND("chamando recarga 2 UAV");      

        Simulator::Schedule (Seconds(1),
                                 &BasicEnergySource::Recharge,
                                 this);


}


bool BasicEnergySource::GetRechargeStatus(){

        return recharged;
}


//x é um valor fixo representando gasto com processamento, a ser definido pelo usuario durante o uso da função
void BasicEnergySource::ProcessEnergy(double x){

  if (m_remainingEnergyJ >= x)
  {
     m_remainingEnergyJ -= x;
  }else{
    NS_LOG_UNCOND("ENERGIA BAIXA");
  }

}

//Calculo de energia por mobilidade
//Ordem de entrada dos parametros: posição X, posição Y, posição Z, tempo de atualização, velocidade
void
BasicEnergySource::UpdateEnergyMobSource (double x, double y, double z, float time, float speed)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("BasicEnergySource:Updating remaining energy.");

  double remainingEnergy = m_remainingEnergyJ;
  CalculateRemainingEnergy ();
  // NS_LOG_UNCOND("X" << AtualPosX);
  //NS_LOG_UNCOND("y" << AtualPosY);
  //NS_LOG_UNCOND("Z" << AtualPosZ);

  if(AtualPosX < 0){
    //holder = Simulator::Now ();
    //tempo_atual = holder.GetSeconds();
    AtualPosX = x;
    AtualPosY = y;
    AtualPosZ = z;

    //  NS_LOG_UNCOND("X" << AtualPosX);
    //  NS_LOG_UNCOND("y" << AtualPosY);
    //  NS_LOG_UNCOND("Z" << AtualPosZ);
    return;
  }


 

  
  float distance;

  float part1 = (AtualPosX - x)*(AtualPosX - x);
   //NS_LOG_UNCOND(part1);
  float part2 = (AtualPosY - y)*(AtualPosY - y);
   //NS_LOG_UNCOND(part2);

  distance = sqrt(part1 + part2); 
  //NS_LOG_UNCOND("distancia percorrida");
  //NS_LOG_UNCOND(distance);

  float updown = z - AtualPosZ;

  AtualPosX = x;
  AtualPosY = y;
  AtualPosZ = z;

  

  float energy_spent;

  //float tempo_passado = tempo_atual;
  //float tempo_atual = holder.GetSeconds();


  //Considerando hovering como uma variação muito baixa de posicionamento, neste caso igual ou abaixo de meio metro
  if(distance > 0.5){
    //NS_LOG_UNCOND("mobilidade feita, distacia percorrida no tempo:");
    //NS_LOG_UNCOND(distance);
    
    //Implementar calculo de P * (D/V) || V pode ser abstraido caso necessario como D/T
    //ou seja o espaço percorrido no intervalo de tempo da tualização
    //P = Consumo de velocidade constante (Considerando o móduo de velocidade constante, adaptar caso contrario)
    //D = Distancia calculada, V = velocidade 

    float p = 200;
    //float speed = 4; //M/s

    float gasto = p * (distance/speed);
    energy_spent = gasto;
    //NS_LOG_UNCOND(gasto);



  }else{
    //NS_LOG_UNCOND("UAV em hovering");

    //p =??

    //float energy_cons = p * 

    // implementar calculo de P * (T2 - t1)
    //P = Consumo de hovering, T1 = tempo atual, t2 = tempo da ultima medição
    float p = 10;

    float gasto = p * time;
    energy_spent = gasto;
    //NS_LOG_UNCOND(gasto);


  }

  if(updown !=0){
    //NS_LOG_UNCOND("UAV SUBDINO OU DESCENDO");

    if(updown > 0){
      //NS_LOG_UNCOND("UAV subindo");
      //Variavel de gasto fixo de subida
      float pc = 5;
      energy_spent += (pc * (updown/speed));

    }else{
      NS_LOG_UNCOND("UAV descendo");
      //Variavel de gasto fixo de descida
      float pd = 3;
      energy_spent += ((pd * (-1)) * (updown/speed));

    }
  }

  

  //NS_LOG_UNCOND("ENERGIA INICIAL:");
  //NS_LOG_UNCOND(GetInitialEnergy());

  if (m_remainingEnergyJ >= energy_spent)
  {
     m_remainingEnergyJ -= energy_spent;
  }else{
    NS_LOG_UNCOND("ENERGIA BAIXA");
  }

 

  //NS_LOG_UNCOND("ENERGIA RESTANTE:");
  //NS_LOG_UNCOND(m_remainingEnergyJ);

  if (!m_depleted && m_remainingEnergyJ <= m_lowBatteryTh * m_initialEnergyJ)
    {
      m_depleted = true;
      HandleEnergyDrainedEvent ();
    }
  else if (m_depleted && m_remainingEnergyJ > m_highBatteryTh * m_initialEnergyJ)
    {
      m_depleted = false;
      HandleEnergyRechargedEvent ();
    }
  else if (m_remainingEnergyJ != remainingEnergy)
    {
      NotifyEnergyChanged ();
    }

  if (m_energyUpdateEvent.IsExpired ())
    {
      m_energyUpdateEvent = Simulator::Schedule (m_energyUpdateInterval,
                                                 &BasicEnergySource::UpdateEnergySource,
                                                 this);
    }



}

/*
 * Private functions start here.
 */

void
BasicEnergySource::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  UpdateEnergySource ();  // start periodic update
}

void
BasicEnergySource::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  BreakDeviceEnergyModelRefCycle ();  // break reference cycle
}

void
BasicEnergySource::HandleEnergyDrainedEvent (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("BasicEnergySource:Energy depleted!");
  NotifyEnergyDrained (); // notify DeviceEnergyModel objects
}

void
BasicEnergySource::HandleEnergyRechargedEvent (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("BasicEnergySource:Energy recharged!");
  NotifyEnergyRecharged (); // notify DeviceEnergyModel objects
}

void
BasicEnergySource::CalculateRemainingEnergy (void)
{
  NS_LOG_FUNCTION (this);
  double totalCurrentA = CalculateTotalCurrent ();
  Time duration = Simulator::Now () - m_lastUpdateTime;
  NS_ASSERT (duration.IsPositive ());
  // energy = current * voltage * time
  double energyToDecreaseJ = (totalCurrentA * m_supplyVoltageV * duration.GetNanoSeconds ()) / 1e9;
  if (m_remainingEnergyJ <= energyToDecreaseJ)
  {
    energyToDecreaseJ = m_remainingEnergyJ;
  }
  NS_ASSERT (m_remainingEnergyJ >= energyToDecreaseJ);
  m_remainingEnergyJ -= energyToDecreaseJ;
  NS_LOG_DEBUG ("BasicEnergySource:Remaining energy = " << m_remainingEnergyJ);
}

} // namespace ns3
