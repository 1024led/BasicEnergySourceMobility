# BasicEnergySourceMobility
(Faltando adicionar instruções de uso de outras funcionalidades)
Modulo de energia "BasicEnergySource" implementado no NS3 com compatibilidade a gasto por mobilidade para UAV's (em testes).

funcionalidades implementadas:
- Gasto de energia com mobilidade (Horizontal, Vertical e Hovering);
- Função para desconto de processamento (a ser adicionado)
- Função de recarga 

Necessaria a adição da seginte função para integrar com o codigo principal.

---------------------------------------------------------------------------------------------------

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
	      NS_LOG_UNCOND("Numero de nos ativos:");
	      NS_LOG_UNCOND(UABSNodes.GetN());

	      Simulator::Schedule(Seconds(1), &UpdateMobilityEnergy, UABSNodes);

	}
  
  ----------------------------------------------------------------------------------------------------------------
  
  

