--[[
	for all algorithm : 
	void aif.getTurn(game_state) : return 1 for P1 and return 2 for P2
	int aif.getNumberOfMoves(game_state) : return how many legal moves the current player have
	string aif.simulate(game_state, move_index) : return new game_state given a game_state and a move number move_index(zero-based index)
	int aif.whoWin(game_state) : return 1 for P1, return -1 for P2, return 0 for draw, and return 2 if the game is not finished yet  
	
	for all algorithm with heuristic : 
	double evaluate(game_state) : return heuristic value for given game_state, if the winner is P1, it should return Double.MAX_VALUE, and if the winner is P2, it should return Double.MIN_VALUE  
]]

math.randomseed(os.time())
local random = math.random
local is_log = true

function log(...) 
	if(is_log) then
		print(...)
	end
end

local g_monteCarlo = {
	map = {}, 
	size = 0, 
	time = 3, --const
}

local g_miniMax = {
	depth = 4,  --const,
}

globals_ai = {}
globals_ai.g_monteCarlo = g_monteCarlo
globals_ai.g_miniMax = g_miniMax

function monteCarloCreateNode(value, parent)
	return {value = value, parent = parent, result = 0, visit = 0, childs = {}}
end

function monteCarloSelect(node)		
	local num_moves = aif.getNumberOfMoves(node.value)

	if(num_moves == 0) then
		return node, -1
	end

	local move_index = random(num_moves)-1	
	local new_game_state = nil
	local selected_node = nil

	if(node.childs[move_index] ~= nil) then		
		--print("already exist", move_index)		
		return node.childs[move_index], move_index
	end

	new_game_state = aif.simulate(node.value, move_index)		
	
	if(g_monteCarlo.map[new_game_state] ~= nil) then
		node.childs[move_index] = g_monteCarlo.map[new_game_state]
		return g_monteCarlo.map[new_game_state], move_index
	end
		
	--print("not exist yet", move_index)	
	selected_node = monteCarloCreateNode(new_game_state, node)				
	return selected_node, move_index
end

function monteCarloExpand(child, move_index, parent)	
	g_monteCarlo.map[child.value] = child
	child.parent = parent	
	parent.childs[move_index] = child	
	g_monteCarlo.size = g_monteCarlo.size + 1	
end

function monteCarloSimulate(node)
	local current_node = node
	local num_moves = nil
	local new_game_state = nil
	local result = aif.whoWin(current_node.value)	

	while(result == 2) do
		num_moves = aif.getNumberOfMoves(current_node.value)
		new_game_state = aif.simulate(current_node.value, random(num_moves)-1)
		current_node = monteCarloCreateNode(new_game_state, current_node)		
		result = aif.whoWin(current_node.value)		
	end	

	--print("result of simulation : ", result)
	return result
end

function monteCarloBackPropagation(node, result)
	node.result = node.result + result
	node.visit = node.visit + 1
end

function monteCarloSelectFinal(node)
	local best_move_index = nil	
	local new_game_state = nil
	local turn = aif.getTurn(node.value)
	local best_move_avg = nil
	local current_avg = nil
	
	for k, v in pairs(node.childs) do		
		current_avg = v.result / v.visit
		--print(k, turn, v.result, v.visit, current_avg)
		if ((best_move_avg == nil) or (turn == 1 and current_avg > best_move_avg) or (turn == 2 and current_avg < best_move_avg)) then
			best_move_avg = current_avg
			best_move_index = k
		end
		--print(current_avg, k, best_move_index)
	end		
	
	assert(best_move_index, "error on ai.lua : (monteCarlo) no move selected")	
	return best_move_index
end

--menerima state game_state dengan jumlah kemungkinan move sebanyak num_moves dengan waktu proses maksimum sebanyak time
function monteCarlo(game_state, num_moves) 	
	
	local start_time = os.time()
	local time = g_monteCarlo.time
	local root_node = nil
	if(g_monteCarlo.map[game_state] ~= nil) then
		root_node = g_monteCarlo.map[game_state]
	else		
		root_node = monteCarloCreateNode(game_state, nil)
		g_monteCarlo.map[root_node.value] = root_node
	end			
	local count = 0
	local current_node = nil
	local last_node = nil
	local move_index = nil

	while(os.time() - start_time < time) do		
		current_node = root_node
		
		while(move_index ~= -1 and g_monteCarlo.map[current_node.value] ~= nil) do			
			last_node = current_node
			current_node, move_index = monteCarloSelect(current_node)
		end

		if(move_index ~= -1) then			
			monteCarloExpand(current_node, move_index, last_node)
			local result = monteCarloSimulate(current_node) --simulate until terminal node
			count = count + 1
			while(current_node ~= nil) do
				monteCarloBackPropagation(current_node, result)
				current_node = current_node.parent
			end					
		end
		
		--print(os.time() - start_time)
	end	
	local best_move = monteCarloSelectFinal(root_node)
	log("best_move : ", best_move, "tree size : ",g_monteCarlo.size, "current sim count : ", count)	
	return best_move	
end

--diasumsikan player selalu jalan gantian, tidak berarti bahwa game tidak bisa membolehkan player jalan lebih dari satu
--kali berturut2, hanya saja berarti lawannya harus memilih langkah pass
function miniMax(game_state)
	local node = miniMaxCreateNode(game_state)
	local value, move_index = miniMaxRec(node, g_miniMax.depth, math.huge * -1, math.huge)
	return move_index
end

function miniMaxCreateNode(state)
	return {state = state}
end

function miniMaxRec(node, depth, min, max)	
	--log("visit", node.index, depth, min, max)
	if(depth == 0) then
		--terminal node		
		local ret = aif.evaluate(node.state)				
		return ret
	else
		local winner = aif.whoWin(node.state)
		if(winner == 1) then
			return math.huge
		elseif(winner == -1) then
			return math.huge*-1
		end	
	end
	
	local num_of_childs = aif.getNumberOfMoves(node.state)	
	if(aif.getTurn(node.state) == 1) then -- maximizing player
		local v = min
		local v_t = nil
		local move_index = nil				
		for i=0, num_of_childs-1 do
			local child_node = miniMaxCreateNode(aif.simulate(node.state, i))			
			v_t = miniMaxRec(child_node, depth-1, v, max)						
			if(v_t > v) then 
				v = v_t				
				move_index = i 
			end
			if(v >= max) then return max end
		end		
		return v, move_index
	else -- minimizing player
		local v = max
		local v_t = nil
		local move_index = nil
		for i=0, num_of_childs-1 do
			local child_node = miniMaxCreateNode(aif.simulate(node.state, i))
			v_t = miniMaxRec(child_node, depth-1, min, v)			
			if(v_t < v) then 				
				v = v_t				
				move_index = i  
			end
			if(v <= min) then return min end
		end		
		return v, move_index
	end
end