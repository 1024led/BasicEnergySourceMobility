# BasicEnergySourceMobility
(Faltando adicionar instruções de uso de outras funcionalidades)
Modulo de energia "BasicEnergySource" implementado no NS3 com compatibilidade a gasto por mobilidade para UAV's (em testes).

funcionalidades implementadas:
1 - Gasto de energia com mobilidade (Horizontal, Vertical e Hovering);
2 - Eliminação do Nó em caso de energia completamente gasta (integrada à função de gasto por mobilidade); 
3 - Função para desconto de processamento;
4 - Função de recarga (a recarga é realizada ao decorrer do tempo, simulando o tempo de espera para recarregar uma bateria, mais informações abaixo).


IMPORTANTE: Os arquivos devem ser adicionados em /src/energy/model na pasta do NS3.

Necessaria a adição das funções abaixo para integrar com o codigo principal.


1/2 - Gasto de energia com mobilidade
---------------------------------------------------------------------------------------------------
Passe como Referencia o NodeContainer de sua aplicação.
Obs 1: Atenção com as entradas na função UpdateEnergyMobSource, adapte os valores às caracteristicas da sua implementação, principalmente em relação a tempo de atualização.

Obs 2: Dentro da função UpdateEnergyMobSource contida no arquivo basic-energy-source.cc existem as segintes váriaveis de escala do gasto de energia.
"p" representa o gasto para hovering e mobilidade horizontal;
"pc" representa o gasto com mobilidade vertical para cima;
"pd" representa o gasto com mobilidade vertical para baixo.

adaptar esses valores possibilita atenuar ou acentuar o gasto de energia, caso queira uma simulação mais realista busque informações sobre o UAV real que deseja se basear.


//Função de chamada da atualização de energia por mobilidade

	void UpdateMobilityEnergy(NodeContainer &UABSNodes){
	        bool trigger = false;

		for (uint16_t i=0 ; i < UABSNodes.GetN(); i++){
			Ptr<MobilityModel> UABSposition = UABSNodes.Get(i)->GetObject<MobilityModel>();

			Vector pos = UABSposition->GetPosition ();

			Ptr<BasicEnergySource> source = UABSNodes.Get(i)->GetObject<BasicEnergySource>();

			//Ordem de entrada dos parametros: posição X, posição Y, posição Z, tempo de atualização, velocidade
			source->UpdateEnergyMobSource(pos.x,pos.y,pos.z, 1, 4);

			float RE = source->GetRemainingEnergy();

			if(RE == 0){
				trigger = true;
			} 
		}

	      if(trigger){
		      NodeContainer charg_nodes;

		      for (NodeContainer::Iterator j = UABSNodes.Begin ();j != UABSNodes.End (); ++j)
		  {  

		    Ptr<Node> object = *j;
		    Ptr<MobilityModel> UABSposition = object->GetObject<MobilityModel> ();
				      Ptr<BasicEnergySource> source = object->GetObject<BasicEnergySource>();

		    if (source->GetRemainingEnergy() > 0){
					      charg_nodes.Add(object);
				      }
		  }

		      UABSNodes = charg_nodes;

	      }
	      //NS_LOG_UNCOND("Numero de nos ativos:");
	      //NS_LOG_UNCOND(UABSNodes.GetN());

	      Simulator::Schedule(Seconds(1), &UpdateMobilityEnergy, UABSNodes);

	}
  
  
  3 - Função para desconto de processamento
  ----------------------------------------------------------------------------------------------------------------
  Obtenha o BasicEnergySource do nó desejado no NodeContainer, basta chamar a função de desconto de energia. 
  exemplo (utilizando FOR com referencia i):
  
  	Ptr<BasicEnergySource> source = UABSNodes.Get(i)->GetObject<BasicEnergySource>();
	source->ProcessEnergy(X);
	//X sendo o valor de consumo da função executada 
  
  
  4 - Função de recarga 
  ----------------------------------------------------------------------------------------------------------------
  Obtenha o BasicEnergySource do nó desejado no NodeContainer, basta chamar a função de inicio de recarga.
  
  A função CallRecharge faz a chamada do procedimento Recharge em basic-energy-source.cc, este procedimento Recharge recupera uma certa porcentagem de energia a cada segundo,
  caso seja de interesse mudar a intensidade da porcentagem de recuperação de energa, ou utilizar a recuperação em unidades de Joules, esta área deve ser modificada 
  dependendo do interesse do usuario.
  
  exemplo (utilizando FOR com referencia i):
  
 	Ptr<BasicEnergySource> source = UABSNodes.Get(i)->GetObject<BasicEnergySource>();
	source->CallRecharge();
	//X sendo o valor de consumo da função executada 


